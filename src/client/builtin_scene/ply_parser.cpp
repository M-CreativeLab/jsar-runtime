#include "./ply_parser.hpp"
#include <common/debug.hpp>
#include <algorithm>
#include <cstring>

namespace builtin_scene::model_renderer
{
  static const char *LOG_TAG = "PlyParser";

  bool PlyParser::parse(const std::vector<char> &data, std::vector<GaussianSplat> &splats)
  {
    // Convert data to string and split into lines
    std::string content(data.begin(), data.end());
    std::istringstream stream(content);
    std::vector<std::string> lines;
    std::string line;
    
    while (std::getline(stream, line)) {
      lines.push_back(line);
    }
    
    if (lines.empty()) {
      DEBUG(LOG_TAG, "PLY file is empty");
      return false;
    }
    
    // Check PLY magic header
    if (lines[0] != "ply") {
      DEBUG(LOG_TAG, "Invalid PLY file - missing 'ply' header");
      return false;
    }
    
    // Parse header
    std::vector<PlyElement> elements;
    size_t headerEndIndex = 0;
    
    if (!parseHeader(lines, headerEndIndex, elements)) {
      DEBUG(LOG_TAG, "Failed to parse PLY header");
      return false;
    }
    
    // Find vertex element
    PlyElement vertexElement;
    bool foundVertices = false;
    
    for (const auto &element : elements) {
      if (element.name == "vertex") {
        vertexElement = element;
        foundVertices = true;
        break;
      }
    }
    
    if (!foundVertices) {
      DEBUG(LOG_TAG, "No vertex element found in PLY file");
      return false;
    }
    
    // Parse vertex data
    if (!parseVertices(lines, headerEndIndex + 1, vertexElement, splats)) {
      DEBUG(LOG_TAG, "Failed to parse vertex data");
      return false;
    }
    
    DEBUG(LOG_TAG, "Successfully parsed PLY file with %zu splats", splats.size());
    return true;
  }

  bool PlyParser::parseFile(const std::string &filePath, std::vector<GaussianSplat> &splats)
  {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
      DEBUG(LOG_TAG, "Failed to open PLY file: %s", filePath.c_str());
      return false;
    }
    
    // Read file into buffer
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<char> data(fileSize);
    file.read(data.data(), fileSize);
    file.close();
    
    return parse(data, splats);
  }

  bool PlyParser::parseHeader(const std::vector<std::string> &lines, 
                             size_t &headerEndIndex, 
                             std::vector<PlyElement> &elements)
  {
    PlyElement currentElement;
    bool inElement = false;
    
    for (size_t i = 1; i < lines.size(); ++i) {
      const std::string &line = lines[i];
      
      if (line == "end_header") {
        if (inElement) {
          elements.push_back(currentElement);
        }
        headerEndIndex = i;
        return true;
      }
      
      std::vector<std::string> tokens = split(line);
      if (tokens.empty()) continue;
      
      if (tokens[0] == "format") {
        if (tokens.size() >= 2 && tokens[1] != "ascii") {
          DEBUG(LOG_TAG, "Only ASCII PLY format is supported");
          return false;
        }
      }
      else if (tokens[0] == "element") {
        if (inElement) {
          elements.push_back(currentElement);
        }
        
        if (tokens.size() >= 3) {
          currentElement = PlyElement();
          currentElement.name = tokens[1];
          currentElement.count = std::stoi(tokens[2]);
          inElement = true;
        }
      }
      else if (tokens[0] == "property" && inElement) {
        if (tokens.size() >= 3) {
          PlyProperty prop;
          prop.type = tokens[1];
          prop.name = tokens[2];
          prop.index = static_cast<int>(currentElement.properties.size());
          currentElement.properties.push_back(prop);
        }
      }
    }
    
    DEBUG(LOG_TAG, "PLY header incomplete - missing end_header");
    return false;
  }

  bool PlyParser::parseVertices(const std::vector<std::string> &lines,
                               size_t startIndex,
                               const PlyElement &element,
                               std::vector<GaussianSplat> &splats)
  {
    // Create property name to index mapping
    std::map<std::string, int> propMap;
    for (const auto &prop : element.properties) {
      propMap[prop.name] = prop.index;
    }
    
    splats.clear();
    splats.reserve(element.count);
    
    for (int i = 0; i < element.count && (startIndex + i) < lines.size(); ++i) {
      const std::string &line = lines[startIndex + i];
      std::vector<std::string> values = split(line);
      
      if (values.size() < element.properties.size()) {
        DEBUG(LOG_TAG, "Invalid vertex data at line %zu", startIndex + i);
        continue;
      }
      
      GaussianSplat splat;
      
      // Parse position (required)
      if (propMap.count("x")) splat.position[0] = parseFloat(values[propMap["x"]]);
      if (propMap.count("y")) splat.position[1] = parseFloat(values[propMap["y"]]);
      if (propMap.count("z")) splat.position[2] = parseFloat(values[propMap["z"]]);
      
      // Parse color (with defaults)
      splat.color[0] = propMap.count("red") ? parseFloat(values[propMap["red"]], 255.0f) / 255.0f : 1.0f;
      splat.color[1] = propMap.count("green") ? parseFloat(values[propMap["green"]], 255.0f) / 255.0f : 1.0f;
      splat.color[2] = propMap.count("blue") ? parseFloat(values[propMap["blue"]], 255.0f) / 255.0f : 1.0f;
      
      // Alternative RGB naming
      if (propMap.count("r")) splat.color[0] = parseFloat(values[propMap["r"]]);
      if (propMap.count("g")) splat.color[1] = parseFloat(values[propMap["g"]]);
      if (propMap.count("b")) splat.color[2] = parseFloat(values[propMap["b"]]);
      
      // Parse opacity (default to 1.0)
      splat.opacity = propMap.count("opacity") ? parseFloat(values[propMap["opacity"]], 1.0f) : 1.0f;
      if (propMap.count("alpha")) splat.opacity = parseFloat(values[propMap["alpha"]], 1.0f);
      
      // Parse scale (default to 1.0)
      splat.scale[0] = propMap.count("scale_0") ? parseFloat(values[propMap["scale_0"]], 1.0f) : 1.0f;
      splat.scale[1] = propMap.count("scale_1") ? parseFloat(values[propMap["scale_1"]], 1.0f) : 1.0f;
      splat.scale[2] = propMap.count("scale_2") ? parseFloat(values[propMap["scale_2"]], 1.0f) : 1.0f;
      
      // Alternative scale naming
      if (propMap.count("sx")) splat.scale[0] = parseFloat(values[propMap["sx"]], 1.0f);
      if (propMap.count("sy")) splat.scale[1] = parseFloat(values[propMap["sy"]], 1.0f);
      if (propMap.count("sz")) splat.scale[2] = parseFloat(values[propMap["sz"]], 1.0f);
      
      // Parse rotation quaternion (default to identity)
      splat.rotation[0] = propMap.count("rot_0") ? parseFloat(values[propMap["rot_0"]], 0.0f) : 0.0f;
      splat.rotation[1] = propMap.count("rot_1") ? parseFloat(values[propMap["rot_1"]], 0.0f) : 0.0f;
      splat.rotation[2] = propMap.count("rot_2") ? parseFloat(values[propMap["rot_2"]], 0.0f) : 0.0f;
      splat.rotation[3] = propMap.count("rot_3") ? parseFloat(values[propMap["rot_3"]], 1.0f) : 1.0f;
      
      // Alternative rotation naming
      if (propMap.count("qx")) splat.rotation[0] = parseFloat(values[propMap["qx"]], 0.0f);
      if (propMap.count("qy")) splat.rotation[1] = parseFloat(values[propMap["qy"]], 0.0f);
      if (propMap.count("qz")) splat.rotation[2] = parseFloat(values[propMap["qz"]], 0.0f);
      if (propMap.count("qw")) splat.rotation[3] = parseFloat(values[propMap["qw"]], 1.0f);
      
      splats.push_back(splat);
    }
    
    return true;
  }

  std::vector<std::string> PlyParser::split(const std::string &str)
  {
    std::vector<std::string> tokens;
    std::istringstream stream(str);
    std::string token;
    
    while (stream >> token) {
      tokens.push_back(token);
    }
    
    return tokens;
  }

  float PlyParser::parseFloat(const std::string &str, float defaultValue)
  {
    try {
      return std::stof(str);
    } catch (const std::exception &) {
      return defaultValue;
    }
  }
}