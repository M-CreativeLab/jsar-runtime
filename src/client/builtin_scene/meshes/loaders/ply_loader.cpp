#include <common/debug.hpp>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <cstdint>

#include "./ply_loader.hpp"

namespace builtin_scene::model_loaders
{
  static const char *LOG_TAG = "PlyLoader";
  static constexpr float SH_C0 = 0.28209479177387814f;

  bool PlyLoader::decodePly(
    const std::vector<char> &fileBytes,
    std::function<void(int numSplats)> initNumSplats,
    SplatCallback splatCallback)
  {
    try
    {
      size_t headerEnd;
      std::unordered_map<std::string, PlyElement> elements;
      bool littleEndian;

      // Parse header
      if (!parseHeader(fileBytes, headerEnd, elements, littleEndian))
      {
        DEBUG(LOG_TAG, "Failed to parse PLY header");
        return false;
      }

      // Check if we have vertex element (required for splats)
      auto vertexIt = elements.find("vertex");
      if (vertexIt == elements.end())
      {
        DEBUG(LOG_TAG, "No vertex element found in PLY file");
        return false;
      }

      const PlyElement &vertexElement = vertexIt->second;

      // Initialize with number of splats
      initNumSplats(vertexElement.count);

      // Parse vertex data
      const char *binaryData = fileBytes.data() + headerEnd;
      parseElementData(binaryData, vertexElement, littleEndian, [&splatCallback](int index, const std::unordered_map<std::string, float> &properties)
                       { extractSplatData(index, properties, splatCallback); });

      return true;
    }
    catch (const std::exception &e)
    {
      DEBUG(LOG_TAG, "Error decoding PLY file: %s", e.what());
      return false;
    }
  }

  bool PlyLoader::load(const std::vector<char> &data, std::vector<builtin_scene::GaussianSplat> &splats)
  {
    auto initSplats = [&splats](int numSplats)
    {
      splats.clear();
      splats.reserve(numSplats);
    };
    auto addSplat = [&splats](int index,
                              float x,
                              float y,
                              float z,
                              float scaleX,
                              float scaleY,
                              float scaleZ,
                              float quatX,
                              float quatY,
                              float quatZ,
                              float quatW,
                              float opacity,
                              float r,
                              float g,
                              float b)
    {
      builtin_scene::GaussianSplat splat;
      splat.position[0] = x;
      splat.position[1] = -y;
      splat.position[2] = -z;
      splat.scale[0] = scaleX;
      splat.scale[1] = scaleY;
      splat.scale[2] = scaleZ;
      splat.rotation[0] = quatX;
      splat.rotation[1] = -quatY;
      splat.rotation[2] = -quatZ;
      splat.rotation[3] = quatW;
      splat.color[0] = r;
      splat.color[1] = g;
      splat.color[2] = b;
      splat.opacity = opacity;

      splats.push_back(splat);
    };
    return decodePly(data, initSplats, addSplat);
  }

  bool PlyLoader::parseHeader(
    const std::vector<char> &fileBytes,
    size_t &headerEnd,
    std::unordered_map<std::string, PlyElement> &elements,
    bool &littleEndian)
  {
    // Find header terminator
    const std::string headerTerminator = "end_header\n";
    std::string header(fileBytes.begin(), fileBytes.end());
    size_t endPos = header.find(headerTerminator);
    if (endPos == std::string::npos)
    {
      DEBUG(LOG_TAG, "Header terminator not found");
      return false;
    }

    headerEnd = endPos + headerTerminator.length();
    header = header.substr(0, headerEnd);

    // Parse header lines
    std::istringstream headerStream(header);
    std::string line;
    bool firstLine = true;
    PlyElement *currentElement = nullptr;
    littleEndian = true; // Default

    while (std::getline(headerStream, line))
    {
      // Remove carriage return if present
      if (!line.empty() && line.back() == '\r')
      {
        line.pop_back();
      }

      // Skip empty lines
      if (line.empty())
        continue;

      std::istringstream lineStream(line);
      std::string keyword;
      lineStream >> keyword;

      if (firstLine)
      {
        if (keyword != "ply")
        {
          DEBUG(LOG_TAG, "Invalid PLY header - does not start with 'ply'");
          return false;
        }
        firstLine = false;
        continue;
      }

      if (keyword == "format")
      {
        std::string format, version;
        lineStream >> format >> version;

        if (format == "binary_little_endian")
        {
          littleEndian = true;
        }
        else if (format == "binary_big_endian")
        {
          littleEndian = false;
        }
        else
        {
          DEBUG(LOG_TAG, "Unsupported PLY format: %s", format.c_str());
          return false;
        }

        if (version != "1.0")
        {
          DEBUG(LOG_TAG, "Unsupported PLY version: %s", version.c_str());
          return false;
        }
      }
      else if (keyword == "element")
      {
        std::string name;
        int count;
        lineStream >> name >> count;

        PlyElement element;
        element.name = name;
        element.count = count;

        elements[name] = element;
        currentElement = &elements[name];
      }
      else if (keyword == "property")
      {
        if (!currentElement)
        {
          DEBUG(LOG_TAG, "Property found without element");
          return false;
        }

        std::string typeOrList;
        lineStream >> typeOrList;

        PlyProperty property;

        if (typeOrList == "list")
        {
          property.isList = true;
          std::string countTypeStr, typeStr, name;
          lineStream >> countTypeStr >> typeStr >> name;
          property.countType = stringToPropertyType(countTypeStr);
          property.type = stringToPropertyType(typeStr);
          currentElement->properties.push_back(std::make_pair(name, property));
        }
        else
        {
          property.isList = false;
          property.type = stringToPropertyType(typeOrList);
          std::string name;
          lineStream >> name;
          currentElement->properties.push_back(std::make_pair(name, property));
        }
      }
      else if (keyword == "comment")
      {
        // Skip comments
        continue;
      }
      else if (keyword == "end_header")
      {
        break;
      }
    }

    return true;
  }

  size_t PlyLoader::parseElementData(
    const char *data,
    const PlyElement &element,
    bool littleEndian,
    std::function<void(int index, const std::unordered_map<std::string, float> &item)> callback)
  {
    size_t offset = 0;

    for (int i = 0; i < element.count; ++i)
    {
      std::unordered_map<std::string, float> item;

      for (const auto &prop : element.properties)
      {
        const std::string &propName = prop.first;
        const PlyProperty &property = prop.second;

        if (property.isList)
        {
          // Read list count
          float count = parsePropertyValue(data, offset, property.countType, littleEndian);
          offset += getPropertyTypeSize(property.countType);

          // For now, we'll just read the first value of lists or skip them
          // In a full implementation, you might want to handle lists properly
          if (count > 0)
          {
            item[propName] = parsePropertyValue(data, offset, property.type, littleEndian);
          }

          // Skip remaining list items
          offset += static_cast<size_t>(count) * getPropertyTypeSize(property.type);
        }
        else
        {
          item[propName] = parsePropertyValue(data, offset, property.type, littleEndian);
          offset += getPropertyTypeSize(property.type);
        }
      }

      callback(i, item);
    }

    return offset;
  }

  size_t PlyLoader::getPropertyTypeSize(PropertyType type)
  {
    switch (type)
    {
    case PropertyType::CHAR:
    case PropertyType::UCHAR:
      return 1;
    case PropertyType::SHORT:
    case PropertyType::USHORT:
      return 2;
    case PropertyType::INT:
    case PropertyType::UINT:
    case PropertyType::FLOAT:
      return 4;
    case PropertyType::DOUBLE:
      return 8;
    default:
      return 0;
    }
  }

  float PlyLoader::parsePropertyValue(const char *data, size_t offset, PropertyType type, bool littleEndian)
  {
    switch (type)
    {
    case PropertyType::CHAR:
    {
      int8_t value;
      readBinary(data, offset, value, littleEndian);
      return static_cast<float>(value);
    }
    case PropertyType::UCHAR:
    {
      uint8_t value;
      readBinary(data, offset, value, littleEndian);
      return static_cast<float>(value);
    }
    case PropertyType::SHORT:
    {
      int16_t value;
      readBinary(data, offset, value, littleEndian);
      return static_cast<float>(value);
    }
    case PropertyType::USHORT:
    {
      uint16_t value;
      readBinary(data, offset, value, littleEndian);
      return static_cast<float>(value);
    }
    case PropertyType::INT:
    {
      int32_t value;
      readBinary(data, offset, value, littleEndian);
      return static_cast<float>(value);
    }
    case PropertyType::UINT:
    {
      uint32_t value;
      readBinary(data, offset, value, littleEndian);
      return static_cast<float>(value);
    }
    case PropertyType::FLOAT:
    {
      float value;
      readBinary(data, offset, value, littleEndian);
      return value;
    }
    case PropertyType::DOUBLE:
    {
      double value;
      readBinary(data, offset, value, littleEndian);
      return static_cast<float>(value);
    }
    default:
      return 0.0f;
    }
  }

  PlyLoader::PropertyType PlyLoader::stringToPropertyType(const std::string &typeStr)
  {
    if (typeStr == "char")
      return PropertyType::CHAR;
    if (typeStr == "uchar")
      return PropertyType::UCHAR;
    if (typeStr == "short")
      return PropertyType::SHORT;
    if (typeStr == "ushort")
      return PropertyType::USHORT;
    if (typeStr == "int")
      return PropertyType::INT;
    if (typeStr == "uint")
      return PropertyType::UINT;
    if (typeStr == "float")
      return PropertyType::FLOAT;
    if (typeStr == "double")
      return PropertyType::DOUBLE;

    DEBUG(LOG_TAG, "Unknown property type: %s", typeStr.c_str());
    return PropertyType::FLOAT; // Default fallback
  }

  void PlyLoader::extractSplatData(
    int index,
    const std::unordered_map<std::string, float> &properties,
    SplatCallback splatCallback)
  {
    // Extract position (required)
    float x = 0.0f, y = 0.0f, z = 0.0f;
    if (properties.find("x") != properties.end())
      x = properties.at("x");
    if (properties.find("y") != properties.end())
      y = properties.at("y");
    if (properties.find("z") != properties.end())
      z = properties.at("z");

    bool hasScales = properties.find("scale_0") != properties.end() &&
                     properties.find("scale_1") != properties.end() &&
                     properties.find("scale_2") != properties.end();
    bool hasRotations = properties.find("rot_0") != properties.end() &&
                        properties.find("rot_1") != properties.end() &&
                        properties.find("rot_2") != properties.end() &&
                        properties.find("rot_3") != properties.end();

    // Extract scale (with defaults)
    static constexpr float DEFAULT_SCALE = 0.001f;
    float scaleX = hasScales ? std::exp(properties.at("scale_0")) : DEFAULT_SCALE;
    float scaleY = hasScales ? std::exp(properties.at("scale_1")) : DEFAULT_SCALE;
    float scaleZ = hasScales ? std::exp(properties.at("scale_2")) : DEFAULT_SCALE;

    // Extract rotation quaternion (with defaults)
    // Note: Spark uses rot_0 as W component, rot_1 as X, rot_2 as Y, rot_3 as Z
    float quatW = hasRotations ? properties.at("rot_0") : 1.0f; // W component
    float quatX = hasRotations ? properties.at("rot_1") : 0.0f; // X component
    float quatY = hasRotations ? properties.at("rot_2") : 0.0f; // Y component
    float quatZ = hasRotations ? properties.at("rot_3") : 0.0f; // Z component

    // Extract opacity (with default)
    float opacity = 1.0f;
    if (properties.find("opacity") != properties.end())
    {
      opacity = 1.0f / (1.0f + std::exp(-properties.at("opacity"))); // Sigmoid activation
    }

    // Extract color (with defaults)
    float r = 0.5f, g = 0.5f, b = 0.5f;
    if (properties.find("f_dc_0") != properties.end())
      r = 0.5f + SH_C0 * properties.at("f_dc_0");
    if (properties.find("f_dc_1") != properties.end())
      g = 0.5f + SH_C0 * properties.at("f_dc_1");
    if (properties.find("f_dc_2") != properties.end())
      b = 0.5f + SH_C0 * properties.at("f_dc_2");

    // Clamp colors to valid range
    r = std::max(0.0f, std::min(1.0f, r));
    g = std::max(0.0f, std::min(1.0f, g));
    b = std::max(0.0f, std::min(1.0f, b));

    splatCallback(index, x, y, z, scaleX, scaleY, scaleZ, quatX, quatY, quatZ, quatW, opacity, r, g, b);
  }

  template <typename T>
  bool PlyLoader::readBinary(const char *data, size_t offset, T &value, bool littleEndian)
  {
    static_assert(sizeof(T) <= 8, "Unsupported type size");

    // Copy bytes from data
    std::memcpy(&value, data + offset, sizeof(T));

    // Handle endianness if needed for multi-byte types
    if (sizeof(T) > 1)
    {
      // Detect system endianness more reliably
      static const bool systemLittleEndian = []()
      {
        const uint32_t test = 0x01234567;
        return *reinterpret_cast<const uint8_t *>(&test) == 0x67;
      }();

      if (systemLittleEndian != littleEndian)
      {
        // Swap bytes using more efficient approach
        if (sizeof(T) == 2)
        {
          uint16_t *ptr = reinterpret_cast<uint16_t *>(&value);
          *ptr = (*ptr << 8) | (*ptr >> 8);
        }
        else if (sizeof(T) == 4)
        {
          uint32_t *ptr = reinterpret_cast<uint32_t *>(&value);
          *ptr = ((*ptr << 24) & 0xFF000000) |
                 ((*ptr << 8) & 0x00FF0000) |
                 ((*ptr >> 8) & 0x0000FF00) |
                 ((*ptr >> 24) & 0x000000FF);
        }
        else if (sizeof(T) == 8)
        {
          uint64_t *ptr = reinterpret_cast<uint64_t *>(&value);
          *ptr = ((*ptr << 56) & 0xFF00000000000000ULL) |
                 ((*ptr << 40) & 0x00FF000000000000ULL) |
                 ((*ptr << 24) & 0x0000FF0000000000ULL) |
                 ((*ptr << 8) & 0x000000FF00000000ULL) |
                 ((*ptr >> 8) & 0x00000000FF000000ULL) |
                 ((*ptr >> 24) & 0x0000000000FF0000ULL) |
                 ((*ptr >> 40) & 0x000000000000FF00ULL) |
                 ((*ptr >> 56) & 0x00000000000000FFULL);
        }
      }
    }

    return true;
  }

  // Explicit template instantiations for the types we use
  template bool PlyLoader::readBinary<int8_t>(const char *, size_t, int8_t &, bool);
  template bool PlyLoader::readBinary<uint8_t>(const char *, size_t, uint8_t &, bool);
  template bool PlyLoader::readBinary<int16_t>(const char *, size_t, int16_t &, bool);
  template bool PlyLoader::readBinary<uint16_t>(const char *, size_t, uint16_t &, bool);
  template bool PlyLoader::readBinary<int32_t>(const char *, size_t, int32_t &, bool);
  template bool PlyLoader::readBinary<uint32_t>(const char *, size_t, uint32_t &, bool);
  template bool PlyLoader::readBinary<float>(const char *, size_t, float &, bool);
  template bool PlyLoader::readBinary<double>(const char *, size_t, double &, bool);
}