#pragma once

#include <vector>
#include <string>
#include <memory>
#include "./model_3d_renderer.hpp"

namespace builtin_scene::model_renderer
{
  /**
   * Ksplat file parser for 3D Gaussian Splatting models.
   * Supports parsing compact binary .ksplat files with vertex data including position, color, opacity, scale, and rotation.
   */
  class KsplatParser
  {
  public:
    /**
     * Parse a .ksplat file from data buffer and extract Gaussian splats.
     * @param data Raw .ksplat file data
     * @param splats Output vector to store parsed splats
     * @return true if parsing was successful, false otherwise
     */
    static bool parse(const std::vector<char> &data, std::vector<GaussianSplat> &splats);

  private:
    /**
     * Read binary data at the given offset with proper endianness handling.
     * @param data Raw file data
     * @param offset Offset in bytes
     * @param value Output value
     * @return true if read was successful, false if out of bounds
     */
    template <typename T>
    static bool readBinary(const std::vector<char> &data, size_t &offset, T &value);

    /**
     * Validate .ksplat file header.
     * @param data Raw file data
     * @param vertexCount Output vertex count
     * @return true if header is valid
     */
    static bool validateHeader(const std::vector<char> &data, uint32_t &vertexCount);
  };
}