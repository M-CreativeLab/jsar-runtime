#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "./model_3d_renderer.hpp"

namespace builtin_scene::model_renderer
{
  /**
   * Simple PLY file parser for 3D Gaussian Splatting models.
   * Supports parsing PLY files with vertex data including position, color, opacity, scale, and rotation.
   */
  class PlyParser
  {
  public:
    struct PlyProperty
    {
      std::string name;
      std::string type;
      int index;
    };

    struct PlyElement
    {
      std::string name;
      int count;
      std::vector<PlyProperty> properties;
    };

  public:
    /**
     * Parse a PLY file from data buffer and extract Gaussian splats.
     * @param data Raw PLY file data
     * @param splats Output vector to store parsed splats
     * @return true if parsing was successful, false otherwise
     */
    static bool parse(const std::vector<char> &data, std::vector<GaussianSplat> &splats);

    /**
     * Parse a PLY file from file path and extract Gaussian splats.
     * @param filePath Path to the PLY file
     * @param splats Output vector to store parsed splats
     * @return true if parsing was successful, false otherwise
     */
    static bool parseFile(const std::string &filePath, std::vector<GaussianSplat> &splats);

  private:
    /**
     * Parse PLY header and extract element and property information.
     * @param lines All lines from the PLY file
     * @param headerEndIndex Output index where header ends
     * @param elements Output vector of parsed elements
     * @return true if header parsing was successful
     */
    static bool parseHeader(const std::vector<std::string> &lines, 
                           size_t &headerEndIndex, 
                           std::vector<PlyElement> &elements);

    /**
     * Parse vertex data from PLY file lines.
     * @param lines All lines from the PLY file
     * @param startIndex Index where vertex data starts
     * @param element Vertex element description
     * @param splats Output vector to store parsed splats
     * @return true if vertex parsing was successful
     */
    static bool parseVertices(const std::vector<std::string> &lines,
                             size_t startIndex,
                             const PlyElement &element,
                             std::vector<GaussianSplat> &splats);

    /**
     * Split a string by whitespace.
     * @param str Input string
     * @return Vector of split tokens
     */
    static std::vector<std::string> split(const std::string &str);

    /**
     * Convert string to float with error handling.
     * @param str Input string
     * @param defaultValue Default value if conversion fails
     * @return Converted float value
     */
    static float parseFloat(const std::string &str, float defaultValue = 0.0f);
  };
}