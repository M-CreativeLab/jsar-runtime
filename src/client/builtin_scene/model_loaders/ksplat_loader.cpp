#include "./ksplat_loader.hpp"
#include <common/debug.hpp>
#include <cstring>
#include <algorithm>

namespace builtin_scene::model_loaders
{
  static const char *LOG_TAG = "KsplatLoader";

  // .ksplat format constants
  static const uint32_t KSPLAT_MAGIC = 0x6B73706C; // "kspl" in little endian
  static const uint32_t KSPLAT_VERSION = 1;

  bool KsplatLoader::load(const std::vector<char> &data, std::vector<model_renderer::GaussianSplat> &splats)
  {
    try
    {
      uint32_t vertexCount;
      if (!validateHeader(data, vertexCount))
      {
        DEBUG(LOG_TAG, "Invalid .ksplat header");
        return false;
      }

      if (vertexCount == 0)
      {
        DEBUG(LOG_TAG, "No vertices found in .ksplat file");
        return false;
      }

      splats.clear();
      splats.reserve(vertexCount);

      size_t offset = 8; // Skip magic (4 bytes) + vertex count (4 bytes)

      // .ksplat format stores data in the following order for each vertex:
      // - Position: 3 floats (x, y, z)
      // - Scale: 3 floats (sx, sy, sz)
      // - Color: 4 bytes (r, g, b, opacity) as packed uint8_t values
      // - Rotation: 4 floats (quaternion: x, y, z, w)

      const size_t vertexSize = 3 * sizeof(float) +   // position
                                3 * sizeof(float) +   // scale
                                4 * sizeof(uint8_t) + // color + opacity
                                4 * sizeof(float);    // rotation quaternion

      if (data.size() < 8 + vertexCount * vertexSize)
      {
        DEBUG(LOG_TAG, "Insufficient data in .ksplat file. Expected %zu bytes, got %zu", 8 + vertexCount * vertexSize, data.size());
        return false;
      }

      for (uint32_t i = 0; i < vertexCount; ++i)
      {
        model_renderer::GaussianSplat splat;

        // Read position (3 floats)
        if (!readBinary(data, offset, splat.position[0]) ||
            !readBinary(data, offset, splat.position[1]) ||
            !readBinary(data, offset, splat.position[2]))
        {
          DEBUG(LOG_TAG, "Failed to read position for vertex %u", i);
          return false;
        }

        // Read scale (3 floats)
        if (!readBinary(data, offset, splat.scale[0]) ||
            !readBinary(data, offset, splat.scale[1]) ||
            !readBinary(data, offset, splat.scale[2]))
        {
          DEBUG(LOG_TAG, "Failed to read scale for vertex %u", i);
          return false;
        }

        // Read packed color and opacity (4 uint8_t values)
        uint8_t colorData[4];
        if (!readBinary(data, offset, colorData[0]) ||
            !readBinary(data, offset, colorData[1]) ||
            !readBinary(data, offset, colorData[2]) ||
            !readBinary(data, offset, colorData[3]))
        {
          DEBUG(LOG_TAG, "Failed to read color for vertex %u", i);
          return false;
        }

        // Convert packed color from 0-255 to 0.0-1.0 range
        splat.color[0] = colorData[0] / 255.0f;
        splat.color[1] = colorData[1] / 255.0f;
        splat.color[2] = colorData[2] / 255.0f;
        splat.opacity = colorData[3] / 255.0f;

        // Read rotation quaternion (4 floats)
        if (!readBinary(data, offset, splat.rotation[0]) ||
            !readBinary(data, offset, splat.rotation[1]) ||
            !readBinary(data, offset, splat.rotation[2]) ||
            !readBinary(data, offset, splat.rotation[3]))
        {
          DEBUG(LOG_TAG, "Failed to read rotation for vertex %u", i);
          return false;
        }

        splats.push_back(splat);
      }

      DEBUG(LOG_TAG, "Successfully loaded .ksplat file with %zu splats", splats.size());
      return true;
    }
    catch (const std::exception &e)
    {
      DEBUG(LOG_TAG, "Error loading .ksplat file: %s", e.what());
      return false;
    }
  }

  template <typename T>
  bool KsplatLoader::readBinary(const std::vector<char> &data, size_t &offset, T &value)
  {
    if (offset + sizeof(T) > data.size())
    {
      return false;
    }

    std::memcpy(&value, &data[offset], sizeof(T));
    offset += sizeof(T);
    return true;
  }

  bool KsplatLoader::validateHeader(const std::vector<char> &data, uint32_t &vertexCount)
  {
    if (data.size() < 8)
    {
      return false;
    }

    // Check magic number
    uint32_t magic;
    size_t offset = 0;
    if (!readBinary(data, offset, magic) || magic != KSPLAT_MAGIC)
    {
      return false;
    }

    // Read vertex count
    if (!readBinary(data, offset, vertexCount))
    {
      return false;
    }

    return true;
  }

  // Explicit template instantiations for the types we use
  template bool KsplatLoader::readBinary<float>(const std::vector<char> &, size_t &, float &);
  template bool KsplatLoader::readBinary<uint8_t>(const std::vector<char> &, size_t &, uint8_t &);
  template bool KsplatLoader::readBinary<uint32_t>(const std::vector<char> &, size_t &, uint32_t &);
}