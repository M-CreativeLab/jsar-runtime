#include <cstring>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <cstdint>
#include <client/logger.hpp>

#include "./ply_loader.hpp"

namespace builtin_scene::model_loaders
{
  using namespace std;

  static constexpr float SH_C0 = 0.28209479177387814f;

  bool PlyLoader::DecodePly(
    const vector<char> &fileBytes,
    function<void(int numSplats)> initNumSplats,
    SplatCallback splatCallback)
  {
    try
    {
      size_t headerEnd;
      unordered_map<string, PlyElement> elements;
      bool littleEndian;

      // Parse header
      if (!ParseHeader(fileBytes, headerEnd, elements, littleEndian))
      {
        logging::LogError("PlyLoader: Failed to parse PLY header");
        return false;
      }

      // Check if we have vertex element (required for splats)
      auto vertexIt = elements.find("vertex");
      if (vertexIt == elements.end())
      {
        logging::LogError("PlyLoader: No vertex element found in PLY file");
        return false;
      }

      const PlyElement &vertexElement = vertexIt->second;

      // Initialize with number of splats
      initNumSplats(vertexElement.count);

      // Parse vertex data
      const char *binaryData = fileBytes.data() + headerEnd;
      ParseElementData(binaryData,
                       vertexElement,
                       littleEndian,
                       [&splatCallback](int index, const unordered_map<string, float> &properties)
                       { ExtractSplatData(index, properties, splatCallback); });

      return true;
    }
    catch (const exception &e)
    {
      logging::LogError("PlyLoader: Error decoding PLY file - " + string(e.what()));
      return false;
    }
  }

  bool PlyLoader::Load(const vector<char> &data, vector<builtin_scene::GaussianSplat> &splats)
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
    return DecodePly(data, initSplats, addSplat);
  }

  bool PlyLoader::ParseHeader(
    const vector<char> &fileBytes,
    size_t &headerEnd,
    unordered_map<string, PlyElement> &elements,
    bool &littleEndian)
  {
    // Find header terminator
    const string headerTerminator = "end_header\n";
    string header(fileBytes.begin(), fileBytes.end());
    size_t endPos = header.find(headerTerminator);
    if (endPos == string::npos)
    {
      logging::LogError("PlyLoader: Header terminator not found");
      return false;
    }

    headerEnd = endPos + headerTerminator.length();
    header = header.substr(0, headerEnd);

    // Parse header lines
    istringstream headerStream(header);
    string line;
    bool firstLine = true;
    PlyElement *currentElement = nullptr;
    littleEndian = true; // Default

    while (getline(headerStream, line))
    {
      // Remove carriage return if present
      if (!line.empty() && line.back() == '\r')
      {
        line.pop_back();
      }

      // Skip empty lines
      if (line.empty())
        continue;

      istringstream lineStream(line);
      string keyword;
      lineStream >> keyword;

      if (firstLine)
      {
        if (keyword != "ply")
        {
          logging::LogError("PlyLoader: Invalid PLY header - does not start with 'ply'");
          return false;
        }
        firstLine = false;
        continue;
      }

      if (keyword == "format")
      {
        string format, version;
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
          logging::LogError("PlyLoader: Unsupported PLY format - " + format);
          return false;
        }

        if (version != "1.0")
        {
          logging::LogError("PlyLoader: Unsupported PLY version - " + version);
          return false;
        }
      }
      else if (keyword == "element")
      {
        string name;
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
          logging::LogError("PlyLoader: Property found without element");
          return false;
        }

        string typeOrList;
        lineStream >> typeOrList;

        PlyProperty property;

        if (typeOrList == "list")
        {
          property.isList = true;
          string countTypeStr, typeStr, name;
          lineStream >> countTypeStr >> typeStr >> name;
          property.countType = StringToPropertyType(countTypeStr);
          property.type = StringToPropertyType(typeStr);
          currentElement->properties.push_back(make_pair(name, property));
        }
        else
        {
          property.isList = false;
          property.type = StringToPropertyType(typeOrList);
          string name;
          lineStream >> name;
          currentElement->properties.push_back(make_pair(name, property));
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

  size_t PlyLoader::ParseElementData(
    const char *data,
    const PlyElement &element,
    bool littleEndian,
    function<void(int index, const unordered_map<string, float> &item)> callback)
  {
    size_t offset = 0;

    for (int i = 0; i < element.count; ++i)
    {
      unordered_map<string, float> item;

      for (const auto &prop : element.properties)
      {
        const string &propName = prop.first;
        const PlyProperty &property = prop.second;

        if (property.isList)
        {
          // Read list count
          float count = ParsePropertyValue(data, offset, property.countType, littleEndian);
          offset += GetPropertyTypeSize(property.countType);

          // For now, we'll just read the first value of lists or skip them
          // In a full implementation, you might want to handle lists properly
          if (count > 0)
          {
            item[propName] = ParsePropertyValue(data, offset, property.type, littleEndian);
          }

          // Skip remaining list items
          offset += static_cast<size_t>(count) * GetPropertyTypeSize(property.type);
        }
        else
        {
          item[propName] = ParsePropertyValue(data, offset, property.type, littleEndian);
          offset += GetPropertyTypeSize(property.type);
        }
      }

      callback(i, item);
    }

    return offset;
  }

  size_t PlyLoader::GetPropertyTypeSize(PropertyType type)
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

  float PlyLoader::ParsePropertyValue(const char *data, size_t offset, PropertyType type, bool littleEndian)
  {
    switch (type)
    {
    case PropertyType::CHAR:
    {
      int8_t value;
      ReadBinary(data, offset, value, littleEndian);
      return static_cast<float>(value);
    }
    case PropertyType::UCHAR:
    {
      uint8_t value;
      ReadBinary(data, offset, value, littleEndian);
      return static_cast<float>(value);
    }
    case PropertyType::SHORT:
    {
      int16_t value;
      ReadBinary(data, offset, value, littleEndian);
      return static_cast<float>(value);
    }
    case PropertyType::USHORT:
    {
      uint16_t value;
      ReadBinary(data, offset, value, littleEndian);
      return static_cast<float>(value);
    }
    case PropertyType::INT:
    {
      int32_t value;
      ReadBinary(data, offset, value, littleEndian);
      return static_cast<float>(value);
    }
    case PropertyType::UINT:
    {
      uint32_t value;
      ReadBinary(data, offset, value, littleEndian);
      return static_cast<float>(value);
    }
    case PropertyType::FLOAT:
    {
      float value;
      ReadBinary(data, offset, value, littleEndian);
      return value;
    }
    case PropertyType::DOUBLE:
    {
      double value;
      ReadBinary(data, offset, value, littleEndian);
      return static_cast<float>(value);
    }
    default:
      return 0.0f;
    }
  }

  PlyLoader::PropertyType PlyLoader::StringToPropertyType(const string &typeStr)
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

    logging::LogWarning("PlyLoader: Unknown property type - " + typeStr);
    return PropertyType::FLOAT; // Default fallback
  }

  void PlyLoader::ExtractSplatData(
    int index,
    const unordered_map<string, float> &properties,
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
    float scaleX = hasScales ? exp(properties.at("scale_0")) : DEFAULT_SCALE;
    float scaleY = hasScales ? exp(properties.at("scale_1")) : DEFAULT_SCALE;
    float scaleZ = hasScales ? exp(properties.at("scale_2")) : DEFAULT_SCALE;

    // Extract rotation quaternion (with defaults)
    float quatW = hasRotations ? properties.at("rot_0") : 1.0f; // W component
    float quatX = hasRotations ? properties.at("rot_1") : 0.0f; // X component
    float quatY = hasRotations ? properties.at("rot_2") : 0.0f; // Y component
    float quatZ = hasRotations ? properties.at("rot_3") : 0.0f; // Z component

    // Extract opacity (with default)
    float opacity = 1.0f;
    if (properties.find("opacity") != properties.end())
    {
      opacity = 1.0f / (1.0f + exp(-properties.at("opacity"))); // Sigmoid activation
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
    r = max(0.0f, min(1.0f, r));
    g = max(0.0f, min(1.0f, g));
    b = max(0.0f, min(1.0f, b));

    splatCallback(index, x, y, z, scaleX, scaleY, scaleZ, quatX, quatY, quatZ, quatW, opacity, r, g, b);
  }

  template <typename T>
  bool PlyLoader::ReadBinary(const char *data, size_t offset, T &value, bool littleEndian)
  {
    static_assert(sizeof(T) <= 8, "Unsupported type size");

    // Copy bytes from data
    memcpy(&value, data + offset, sizeof(T));

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

  // Progressive loading implementation
  bool PlyLoader::load(const std::vector<char> &data, std::vector<builtin_scene::GaussianSplat> &splats)
  {
    return Load(data, splats);
  }

  bool PlyLoader::initProgressiveLoading(const std::vector<char> &data, ProgressCallback progressCallback)
  {
    // Reset state
    resetProgressiveLoading();

    // Store data and callback
    progressiveData_ = data;
    progressCallback_ = progressCallback;

    // Parse header
    if (!ParseHeader(progressiveData_, headerEnd_, elements_, littleEndian_))
    {
      logging::LogError("PlyLoader: Failed to parse PLY header for progressive loading");
      return false;
    }

    // Check if we have vertex element
    auto vertexIt = elements_.find("vertex");
    if (vertexIt == elements_.end())
    {
      logging::LogError("PlyLoader: No vertex element found in PLY file for progressive loading");
      return false;
    }

    totalSplats_ = vertexIt->second.count;
    loadedSplats_ = 0;
    progressiveInitialized_ = true;

    // Notify progress
    if (progressCallback_)
    {
      progressCallback_(loadedSplats_, totalSplats_);
    }

    return true;
  }

  bool PlyLoader::loadNextBatch(size_t batchSize, std::vector<builtin_scene::GaussianSplat> &splats)
  {
    if (!progressiveInitialized_ || isProgressiveLoadingComplete())
    {
      return false;
    }

    auto vertexIt = elements_.find("vertex");
    if (vertexIt == elements_.end())
    {
      return false;
    }

    const PlyElement &vertexElement = vertexIt->second;
    const char *binaryData = progressiveData_.data() + headerEnd_;

    // Calculate start and end indices for this batch
    int startIndex = loadedSplats_;
    int endIndex = std::min(loadedSplats_ + static_cast<int>(batchSize), totalSplats_);

    // Skip to the correct position in the data
    size_t bytesPerVertex = 0;
    for (const auto &prop : vertexElement.properties)
    {
      const PlyProperty &property = prop.second;
      if (property.isList)
      {
        // For list properties, we can't easily calculate size, so we need to parse from beginning
        // This is a limitation of PLY format for progressive loading
        logging::LogError("PlyLoader: List properties not supported in progressive loading");
        return false;
      }
      else
      {
        bytesPerVertex += GetPropertyTypeSize(property.type);
      }
    }

    // Load batch of vertices
    std::vector<builtin_scene::GaussianSplat> batchSplats;
    batchSplats.reserve(endIndex - startIndex);

    for (int i = startIndex; i < endIndex; ++i)
    {
      size_t vertexOffset = i * bytesPerVertex;

      // Parse single vertex properties
      std::unordered_map<std::string, float> properties;
      size_t propOffset = 0;

      for (const auto &prop : vertexElement.properties)
      {
        const std::string &propName = prop.first;
        const PlyProperty &property = prop.second;

        if (!property.isList)
        {
          float value = ParsePropertyValue(binaryData, vertexOffset + propOffset, property.type, littleEndian_);
          properties[propName] = value;
          propOffset += GetPropertyTypeSize(property.type);
        }
      }

      // Extract splat data
      ExtractSplatData(i, properties, [&batchSplats, this](int index, float x, float y, float z, float scaleX, float scaleY, float scaleZ, float quatX, float quatY, float quatZ, float quatW, float opacity, float r, float g, float b)
                       {
        builtin_scene::GaussianSplat splat = createSplat(
          index, x, -y, -z,  // Apply coordinate system conversion
          scaleX, scaleY, scaleZ,
          quatX, -quatY, -quatZ, quatW,  // Apply quaternion conversion
          opacity, r, g, b);
        batchSplats.push_back(splat); });
    }

    // Append to output vector
    splats.insert(splats.end(), batchSplats.begin(), batchSplats.end());

    // Update progress
    loadedSplats_ = endIndex;

    // Notify progress
    if (progressCallback_)
    {
      progressCallback_(loadedSplats_, totalSplats_);
    }

    return !batchSplats.empty();
  }

  bool PlyLoader::isProgressiveLoadingComplete() const
  {
    return progressiveInitialized_ && loadedSplats_ >= totalSplats_;
  }

  int PlyLoader::getTotalSplatCount() const
  {
    return totalSplats_;
  }

  int PlyLoader::getLoadedSplatCount() const
  {
    return loadedSplats_;
  }

  void PlyLoader::resetProgressiveLoading()
  {
    progressiveData_.clear();
    progressCallback_ = nullptr;
    totalSplats_ = -1;
    loadedSplats_ = 0;
    progressiveInitialized_ = false;
    headerEnd_ = 0;
    elements_.clear();
    littleEndian_ = true;
  }

  bool PlyLoader::loadWithCallback(
    const std::vector<char> &data,
    std::function<void(int numSplats)> initNumSplats,
    SplatCallback splatCallback)
  {
    return DecodePly(data, initNumSplats, splatCallback);
  }

  // Explicit template instantiations for the types we use
  template bool PlyLoader::ReadBinary<int8_t>(const char *, size_t, int8_t &, bool);
  template bool PlyLoader::ReadBinary<uint8_t>(const char *, size_t, uint8_t &, bool);
  template bool PlyLoader::ReadBinary<int16_t>(const char *, size_t, int16_t &, bool);
  template bool PlyLoader::ReadBinary<uint16_t>(const char *, size_t, uint16_t &, bool);
  template bool PlyLoader::ReadBinary<int32_t>(const char *, size_t, int32_t &, bool);
  template bool PlyLoader::ReadBinary<uint32_t>(const char *, size_t, uint32_t &, bool);
  template bool PlyLoader::ReadBinary<float>(const char *, size_t, float &, bool);
  template bool PlyLoader::ReadBinary<double>(const char *, size_t, double &, bool);
}
