#include <common/debug.hpp>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <node/zlib.h>

#include "./spz_loader.hpp"

namespace builtin_scene::model_loaders
{
  static const char *LOG_TAG = "SpzLoader";

  bool SpzLoader::decodeSpz(
    const std::vector<char> &fileBytes,
    std::function<void(int numSplats)> initNumSplats,
    SplatCallback splatCallback)
  {
    try
    {
      // First decompress the gzip data
      std::vector<char> decompressedData;
      if (!decompressGzip(fileBytes, decompressedData))
      {
        DEBUG(LOG_TAG, "Failed to decompress SPZ file");
        return false;
      }

      if (decompressedData.size() < 16)
      {
        DEBUG(LOG_TAG, "SPZ file too small for header. Expected at least 16 bytes, got %zu", decompressedData.size());
        return false;
      }

      const uint8_t *data = reinterpret_cast<const uint8_t *>(decompressedData.data());
      size_t offset = 0;

      // Read and validate header
      uint32_t magic;
      uint32_t version;
      uint32_t numSplats;
      uint8_t shDegree;
      uint8_t fractionalBits;
      uint8_t flags;
      uint8_t reserved;

      memcpy(&magic, &data[offset], sizeof(uint32_t));
      offset += sizeof(uint32_t);
      if (magic != SPZ_MAGIC)
      {
        DEBUG(LOG_TAG, "Invalid SPZ file magic: 0x%08x (expected 0x%08x)", magic, SPZ_MAGIC);
        return false;
      }

      memcpy(&version, &data[offset], sizeof(uint32_t));
      offset += sizeof(uint32_t);
      if (version < 1 || version > 2)
      {
        DEBUG(LOG_TAG, "Unsupported SPZ version: %u", version);
        return false;
      }

      memcpy(&numSplats, &data[offset], sizeof(uint32_t));
      offset += sizeof(uint32_t);
      memcpy(&shDegree, &data[offset], sizeof(uint8_t));
      offset += sizeof(uint8_t);
      memcpy(&fractionalBits, &data[offset], sizeof(uint8_t));
      offset += sizeof(uint8_t);
      memcpy(&flags, &data[offset], sizeof(uint8_t));
      offset += sizeof(uint8_t);
      memcpy(&reserved, &data[offset], sizeof(uint8_t));
      offset += sizeof(uint8_t);

      if (numSplats == 0)
      {
        DEBUG(LOG_TAG, "No splats found in SPZ file");
        return false;
      }

      // Initialize with total splat count
      initNumSplats(numSplats);

      // Read centers
      std::vector<float> centers(numSplats * 3);
      if (version == 1)
      {
        // Version 1: float16 centers
        const size_t centerBytesNeeded = numSplats * 3 * 2;
        if (offset + centerBytesNeeded > decompressedData.size())
        {
          DEBUG(LOG_TAG, "SPZ file too small for centers");
          return false;
        }

        const uint16_t *centerUint16 = reinterpret_cast<const uint16_t *>(&data[offset]);
        for (uint32_t i = 0; i < numSplats; i++)
        {
          const uint32_t i3 = i * 3;
          centers[i3] = fromHalf(centerUint16[i3]);
          centers[i3 + 1] = fromHalf(centerUint16[i3 + 1]);
          centers[i3 + 2] = fromHalf(centerUint16[i3 + 2]);
        }
        offset += centerBytesNeeded;
      }
      else if (version == 2)
      {
        // Version 2: 24-bit fixed-point centers
        const uint32_t fixed = 1 << fractionalBits;
        const size_t centerBytesNeeded = numSplats * 3 * 3;
        if (offset + centerBytesNeeded > decompressedData.size())
        {
          DEBUG(LOG_TAG, "SPZ file too small for centers");
          return false;
        }

        for (uint32_t i = 0; i < numSplats; i++)
        {
          const uint32_t i9 = i * 9;
          const uint32_t i3 = i * 3;

          // Read 3 bytes per coordinate, sign-extend to 32-bit
          const int32_t x = (static_cast<int32_t>((data[offset + i9 + 2] << 24) |
                                                  (data[offset + i9 + 1] << 16) |
                                                  (data[offset + i9] << 8)) >>
                             8);
          const int32_t y = (static_cast<int32_t>((data[offset + i9 + 5] << 24) |
                                                  (data[offset + i9 + 4] << 16) |
                                                  (data[offset + i9 + 3] << 8)) >>
                             8);
          const int32_t z = (static_cast<int32_t>((data[offset + i9 + 8] << 24) |
                                                  (data[offset + i9 + 7] << 16) |
                                                  (data[offset + i9 + 6] << 8)) >>
                             8);

          centers[i3] = static_cast<float>(x) / fixed;
          centers[i3 + 1] = static_cast<float>(y) / fixed;
          centers[i3 + 2] = static_cast<float>(z) / fixed;
        }
        offset += centerBytesNeeded;
      }

      // Read alpha values
      if (offset + numSplats > decompressedData.size())
      {
        DEBUG(LOG_TAG, "SPZ file too small for alpha values");
        return false;
      }
      std::vector<float> alphas(numSplats);
      for (uint32_t i = 0; i < numSplats; i++)
      {
        alphas[i] = data[offset + i] / 255.0f;
      }
      offset += numSplats;

      // Read RGB values
      const size_t rgbBytesNeeded = numSplats * 3;
      if (offset + rgbBytesNeeded > decompressedData.size())
      {
        DEBUG(LOG_TAG, "SPZ file too small for RGB values");
        return false;
      }
      std::vector<float> colors(numSplats * 3);
      const float scale = SH_C0 / 0.15f;
      for (uint32_t i = 0; i < numSplats; i++)
      {
        const uint32_t i3 = i * 3;
        colors[i3] = (data[offset + i3] / 255.0f - 0.5f) * scale + 0.5f;
        colors[i3 + 1] = (data[offset + i3 + 1] / 255.0f - 0.5f) * scale + 0.5f;
        colors[i3 + 2] = (data[offset + i3 + 2] / 255.0f - 0.5f) * scale + 0.5f;
      }
      offset += rgbBytesNeeded;

      // Read scales
      const size_t scalesBytesNeeded = numSplats * 3;
      if (offset + scalesBytesNeeded > decompressedData.size())
      {
        DEBUG(LOG_TAG, "SPZ file too small for scale values");
        return false;
      }
      std::vector<float> scales(numSplats * 3);
      for (uint32_t i = 0; i < numSplats; i++)
      {
        const uint32_t i3 = i * 3;
        scales[i3] = std::exp(data[offset + i3] / 16.0f - 10.0f);
        scales[i3 + 1] = std::exp(data[offset + i3 + 1] / 16.0f - 10.0f);
        scales[i3 + 2] = std::exp(data[offset + i3 + 2] / 16.0f - 10.0f);
      }
      offset += scalesBytesNeeded;

      // Read quaternions
      const size_t quatBytesNeeded = numSplats * 3;
      if (offset + quatBytesNeeded > decompressedData.size())
      {
        DEBUG(LOG_TAG, "SPZ file too small for quaternion values");
        return false;
      }
      std::vector<float> quaternions(numSplats * 4);
      for (uint32_t i = 0; i < numSplats; i++)
      {
        const uint32_t i3 = i * 3;
        const uint32_t i4 = i * 4;

        const float quatX = data[offset + i3] / 127.5f - 1.0f;
        const float quatY = data[offset + i3 + 1] / 127.5f - 1.0f;
        const float quatZ = data[offset + i3 + 2] / 127.5f - 1.0f;
        const float quatW = std::sqrt(std::max(0.0f, 1.0f - quatX * quatX - quatY * quatY - quatZ * quatZ));

        quaternions[i4] = quatX;
        quaternions[i4 + 1] = quatY;
        quaternions[i4 + 2] = quatZ;
        quaternions[i4 + 3] = quatW;
      }
      offset += quatBytesNeeded;

      // Call the splat callback for each splat
      for (uint32_t i = 0; i < numSplats; i++)
      {
        const uint32_t i3 = i * 3;
        const uint32_t i4 = i * 4;

        // Convert from 3DGS coordinate system (Y-down) to OpenGL coordinate system (Y-up)
        // This fixes the Y-axis flip issue commonly seen when loading 3DGS datasets
        float convertedY = -centers[i3 + 1];

        // For rotation quaternion, we need to adjust for the Y-flip
        // When flipping Y-axis, we need to negate the Y and Z components of the quaternion
        float convertedQuatY = -quaternions[i4 + 1];
        float convertedQuatZ = -quaternions[i4 + 2];

        splatCallback(
          i,
          centers[i3],
          convertedY,
          centers[i3 + 2],
          scales[i3],
          scales[i3 + 1],
          scales[i3 + 2],
          quaternions[i4],
          convertedQuatY,
          convertedQuatZ,
          quaternions[i4 + 3],
          alphas[i],
          colors[i3],
          colors[i3 + 1],
          colors[i3 + 2]);
      }

      DEBUG(LOG_TAG, "Successfully decoded SPZ file with %u splats", numSplats);
      return true;
    }
    catch (const std::exception &e)
    {
      DEBUG(LOG_TAG, "Error decoding SPZ file: %s", e.what());
      return false;
    }
  }

  bool SpzLoader::load(const std::vector<char> &data, std::vector<builtin_scene::GaussianSplat> &splats)
  {
    splats.clear();

    return decodeSpz(
      data,
      [&splats](int numSplats)
      {
        splats.reserve(numSplats);
      },
      [&splats](int index, float x, float y, float z, float scaleX, float scaleY, float scaleZ, float quatX, float quatY, float quatZ, float quatW, float opacity, float r, float g, float b)
      {
        builtin_scene::GaussianSplat splat;
        // Note: Coordinate conversion is already applied in the callback from decodeSpz
        splat.position[0] = x;
        splat.position[1] = y;
        splat.position[2] = z;
        splat.scale[0] = scaleX;
        splat.scale[1] = scaleY;
        splat.scale[2] = scaleZ;
        splat.rotation[0] = quatX;
        splat.rotation[1] = quatY;
        splat.rotation[2] = quatZ;
        splat.rotation[3] = quatW;
        splat.color[0] = r;
        splat.color[1] = g;
        splat.color[2] = b;
        splat.opacity = opacity;
        splats.push_back(splat);
      });
  }

  bool SpzLoader::decompressGzip(const std::vector<char> &compressedData, std::vector<char> &decompressedData)
  {
    if (compressedData.empty())
    {
      DEBUG(LOG_TAG, "Empty compressed data");
      return false;
    }

    z_stream stream;
    memset(&stream, 0, sizeof(stream));

    // Initialize for gzip decompression (windowBits = 15 + 16 for gzip)
    if (inflateInit2(&stream, 15 + 16) != Z_OK)
    {
      DEBUG(LOG_TAG, "Failed to initialize zlib for gzip decompression");
      return false;
    }

    stream.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(compressedData.data()));
    stream.avail_in = compressedData.size();

    // Start with a reasonable buffer size and grow as needed
    const size_t chunkSize = 16384;
    decompressedData.clear();
    decompressedData.reserve(compressedData.size() * 4); // Initial guess

    int ret;
    do
    {
      // Expand output buffer
      size_t currentSize = decompressedData.size();
      decompressedData.resize(currentSize + chunkSize);

      stream.next_out = reinterpret_cast<Bytef *>(decompressedData.data() + currentSize);
      stream.avail_out = chunkSize;

      ret = inflate(&stream, Z_NO_FLUSH);

      if (ret != Z_OK && ret != Z_STREAM_END)
      {
        DEBUG(LOG_TAG, "Gzip decompression failed with error: %d", ret);
        inflateEnd(&stream);
        return false;
      }

      // Adjust actual size
      decompressedData.resize(currentSize + chunkSize - stream.avail_out);

    } while (ret != Z_STREAM_END && stream.avail_in > 0);

    inflateEnd(&stream);

    if (ret != Z_STREAM_END)
    {
      DEBUG(LOG_TAG, "Incomplete gzip decompression");
      return false;
    }

    DEBUG(LOG_TAG, "Successfully decompressed gzip data: %zu -> %zu bytes", compressedData.size(), decompressedData.size());
    return true;
  }

  float SpzLoader::fromHalf(uint16_t value)
  {
    // IEEE 754 half-float to single-float conversion
    uint32_t sign = (value & 0x8000) << 16;
    uint32_t exponent = (value & 0x7C00) >> 10;
    uint32_t mantissa = value & 0x03FF;

    if (exponent == 0)
    {
      if (mantissa == 0)
      {
        // Zero
        uint32_t result = sign;
        return *reinterpret_cast<float *>(&result);
      }
      else
      {
        // Denormalized number
        exponent = 127 - 15;
        while ((mantissa & 0x400) == 0)
        {
          mantissa <<= 1;
          exponent--;
        }
        mantissa &= 0x3FF;
      }
    }
    else if (exponent == 31)
    {
      // Infinity or NaN
      exponent = 255;
    }
    else
    {
      // Normalized number
      exponent += 127 - 15;
    }

    uint32_t result = sign | (exponent << 23) | (mantissa << 13);
    return *reinterpret_cast<float *>(&result);
  }

  template <typename T>
  bool SpzLoader::readBinary(const std::vector<char> &data, size_t offset, T &value)
  {
    if (offset + sizeof(T) > data.size())
    {
      return false;
    }

    std::memcpy(&value, &data[offset], sizeof(T));
    return true;
  }

  // Explicit template instantiations for the types we use
  template bool SpzLoader::readBinary<float>(const std::vector<char> &, size_t, float &);
  template bool SpzLoader::readBinary<uint8_t>(const std::vector<char> &, size_t, uint8_t &);
  template bool SpzLoader::readBinary<uint16_t>(const std::vector<char> &, size_t, uint16_t &);
  template bool SpzLoader::readBinary<uint32_t>(const std::vector<char> &, size_t, uint32_t &);
}