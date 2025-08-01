#pragma once

#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <tinyply/tinyply.h>
#include "./model_3d_renderer.hpp"

namespace builtin_scene::model_renderer
{
  /**
   * PLY file parser for 3D Gaussian Splatting models using tinyply library.
   * Supports parsing PLY files with vertex data including position, color, opacity, scale, and rotation.
   */
  class PlyParser
  {
  public:
    /**
     * Parse a PLY file from data buffer and extract Gaussian splats.
     * @param data Raw PLY file data
     * @param splats Output vector to store parsed splats
     * @return true if parsing was successful, false otherwise
     */
    static bool parse(const std::vector<char> &data, std::vector<GaussianSplat> &splats);

  private:
    /**
     * Extract float value from tinyply data at given index.
     * @param plyData Tinyply data object
     * @param index Index of the value to extract
     * @param defaultValue Default value if extraction fails
     * @return Extracted float value
     */
    static float extractFloat(const std::shared_ptr<tinyply::PlyData> &plyData, 
                             size_t index, 
                             float defaultValue = 0.0f);
  };
}