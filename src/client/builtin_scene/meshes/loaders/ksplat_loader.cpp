#include <common/debug.hpp>
#include <cstring>
#include <algorithm>
#include <cmath>

#include "./ksplat_loader.hpp"

namespace builtin_scene::model_loaders
{
  static const char *LOG_TAG = "KsplatLoader";

  // Compression level definitions following Spark implementation
  const KsplatLoader::KsplatCompression KsplatLoader::KSPLAT_COMPRESSION[3] = {
    // Level 0: Uncompressed
    {
      .bytesPerCenter = 12,
      .bytesPerScale = 12,
      .bytesPerRotation = 16,
      .bytesPerColor = 4,
      .bytesPerSphericalHarmonicsComponent = 4,
      .scaleOffsetBytes = 12,
      .rotationOffsetBytes = 24,
      .colorOffsetBytes = 40,
      .sphericalHarmonicsOffsetBytes = 44,
      .scaleRange = 1,
    },
    // Level 1: Half-float compression
    {
      .bytesPerCenter = 6,
      .bytesPerScale = 6,
      .bytesPerRotation = 8,
      .bytesPerColor = 4,
      .bytesPerSphericalHarmonicsComponent = 2,
      .scaleOffsetBytes = 6,
      .rotationOffsetBytes = 12,
      .colorOffsetBytes = 20,
      .sphericalHarmonicsOffsetBytes = 24,
      .scaleRange = 32767,
    },
    // Level 2: 8-bit compression
    {
      .bytesPerCenter = 6,
      .bytesPerScale = 6,
      .bytesPerRotation = 8,
      .bytesPerColor = 4,
      .bytesPerSphericalHarmonicsComponent = 1,
      .scaleOffsetBytes = 6,
      .rotationOffsetBytes = 12,
      .colorOffsetBytes = 20,
      .sphericalHarmonicsOffsetBytes = 24,
      .scaleRange = 32767,
    }};

  bool KsplatLoader::decodeKsplat(
    const std::vector<char> &fileBytes,
    std::function<void(int numSplats)> initNumSplats,
    SplatCallback splatCallback)
  {
    try
    {
      if (fileBytes.size() < HEADER_BYTES)
      {
        DEBUG(LOG_TAG, "File too small for KSplat header. Expected %zu bytes, got %zu", HEADER_BYTES, fileBytes.size());
        return false;
      }

      // Parse header following Spark implementation
      const uint8_t *data = reinterpret_cast<const uint8_t *>(fileBytes.data());
      size_t headerOffset = 0;

      const uint8_t versionMajor = data[0];
      const uint8_t versionMinor = data[1];
      if (versionMajor != 0 || versionMinor < 1)
      {
        DEBUG(LOG_TAG, "Unsupported .ksplat version: %d.%d", versionMajor, versionMinor);
        return false;
      }

      uint32_t maxSectionCount, splatCount;
      uint16_t compressionLevel;
      memcpy(&maxSectionCount, &data[4], sizeof(uint32_t));
      memcpy(&splatCount, &data[16], sizeof(uint32_t));
      memcpy(&compressionLevel, &data[20], sizeof(uint16_t));

      if (compressionLevel < 0 || compressionLevel > 2)
      {
        DEBUG(LOG_TAG, "Invalid .ksplat compression level: %d", compressionLevel);
        return false;
      }

      if (splatCount == 0)
      {
        DEBUG(LOG_TAG, "No splats found in KSplat file");
        return false;
      }

      // Initialize with total splat count
      initNumSplats(splatCount);

      headerOffset = HEADER_BYTES;
      size_t sectionBase = HEADER_BYTES + maxSectionCount * SECTION_BYTES;

      // Process each section
      for (uint32_t section = 0; section < maxSectionCount; ++section)
      {
        if (headerOffset + SECTION_BYTES > fileBytes.size())
        {
          DEBUG(LOG_TAG, "File too small for section header");
          return false;
        }

        const uint8_t *sectionData = &data[headerOffset];
        headerOffset += SECTION_BYTES;

        uint32_t sectionSplatCount, sectionMaxSplatCount, bucketSize, bucketCount;
        float bucketBlockSize;
        uint16_t bucketStorageSizeBytes;
        uint32_t compressionScaleRange, fullBucketCount, partiallyFilledBucketCount;

        memcpy(&sectionSplatCount, &sectionData[0], sizeof(uint32_t));
        memcpy(&sectionMaxSplatCount, &sectionData[4], sizeof(uint32_t));
        memcpy(&bucketSize, &sectionData[8], sizeof(uint32_t));
        memcpy(&bucketCount, &sectionData[12], sizeof(uint32_t));
        memcpy(&bucketBlockSize, &sectionData[16], sizeof(float));
        memcpy(&bucketStorageSizeBytes, &sectionData[20], sizeof(uint16_t));
        memcpy(&compressionScaleRange, &sectionData[24], sizeof(uint32_t));
        memcpy(&fullBucketCount, &sectionData[32], sizeof(uint32_t));
        memcpy(&partiallyFilledBucketCount, &sectionData[36], sizeof(uint32_t));

        // Use default scale range if not specified
        if (compressionScaleRange == 0)
        {
          compressionScaleRange = KSPLAT_COMPRESSION[compressionLevel].scaleRange;
        }

        const auto &compression = KSPLAT_COMPRESSION[compressionLevel];
        const size_t bytesPerSplat = compression.bytesPerCenter +
                                     compression.bytesPerScale +
                                     compression.bytesPerRotation +
                                     compression.bytesPerColor;

        const size_t bucketsMetaDataSizeBytes = partiallyFilledBucketCount * 4;
        const size_t bucketsStorageSizeBytes = bucketStorageSizeBytes * bucketCount + bucketsMetaDataSizeBytes;
        const size_t splatDataStorageSizeBytes = bytesPerSplat * sectionMaxSplatCount;

        if (sectionSplatCount == 0)
        {
          sectionBase += splatDataStorageSizeBytes + bucketsStorageSizeBytes;
          continue;
        }

        const float compressionScaleFactor = bucketBlockSize / 2.0f / compressionScaleRange;
        const size_t bucketsBase = sectionBase + bucketsMetaDataSizeBytes;
        const size_t dataBase = sectionBase + bucketsStorageSizeBytes;

        if (dataBase + splatDataStorageSizeBytes > fileBytes.size())
        {
          DEBUG(LOG_TAG, "File too small for section data");
          return false;
        }

        const uint8_t *splatData = &data[dataBase];
        const float *bucketArray = reinterpret_cast<const float *>(&data[bucketsBase]);
        const uint32_t *partiallyFilledBucketLengths = reinterpret_cast<const uint32_t *>(&data[sectionBase]);

        const uint32_t fullBucketSplats = fullBucketCount * bucketSize;
        uint32_t partialBucketIndex = fullBucketCount;
        uint32_t partialBucketBase = fullBucketSplats;

        // Process each splat in this section
        for (uint32_t i = 0; i < sectionSplatCount; ++i)
        {
          const size_t splatOffset = i * bytesPerSplat;

          // Determine bucket index
          uint32_t bucketIndex;
          if (i < fullBucketSplats)
          {
            bucketIndex = i / bucketSize;
          }
          else
          {
            const uint32_t bucketLength = partiallyFilledBucketLengths[partialBucketIndex - fullBucketCount];
            if (i >= partialBucketBase + bucketLength)
            {
              partialBucketIndex += 1;
              partialBucketBase += bucketLength;
            }
            bucketIndex = partialBucketIndex;
          }

          // Read position
          float x, y, z;
          if (compressionLevel == 0)
          {
            memcpy(&x, &splatData[splatOffset + 0], sizeof(float));
            memcpy(&y, &splatData[splatOffset + 4], sizeof(float));
            memcpy(&z, &splatData[splatOffset + 8], sizeof(float));
          }
          else
          {
            uint16_t compressedX, compressedY, compressedZ;
            memcpy(&compressedX, &splatData[splatOffset + 0], sizeof(uint16_t));
            memcpy(&compressedY, &splatData[splatOffset + 2], sizeof(uint16_t));
            memcpy(&compressedZ, &splatData[splatOffset + 4], sizeof(uint16_t));

            x = (compressedX - compressionScaleRange) * compressionScaleFactor + bucketArray[3 * bucketIndex + 0];
            y = (compressedY - compressionScaleRange) * compressionScaleFactor + bucketArray[3 * bucketIndex + 1];
            z = (compressedZ - compressionScaleRange) * compressionScaleFactor + bucketArray[3 * bucketIndex + 2];
          }

          // Read scale
          float scaleX, scaleY, scaleZ;
          if (compressionLevel == 0)
          {
            memcpy(&scaleX, &splatData[splatOffset + compression.scaleOffsetBytes + 0], sizeof(float));
            memcpy(&scaleY, &splatData[splatOffset + compression.scaleOffsetBytes + 4], sizeof(float));
            memcpy(&scaleZ, &splatData[splatOffset + compression.scaleOffsetBytes + 8], sizeof(float));
          }
          else
          {
            uint16_t compressedScaleX, compressedScaleY, compressedScaleZ;
            memcpy(&compressedScaleX, &splatData[splatOffset + compression.scaleOffsetBytes + 0], sizeof(uint16_t));
            memcpy(&compressedScaleY, &splatData[splatOffset + compression.scaleOffsetBytes + 2], sizeof(uint16_t));
            memcpy(&compressedScaleZ, &splatData[splatOffset + compression.scaleOffsetBytes + 4], sizeof(uint16_t));

            scaleX = fromHalf(compressedScaleX);
            scaleY = fromHalf(compressedScaleY);
            scaleZ = fromHalf(compressedScaleZ);
          }

          // Read rotation (quaternion) - Spark uses W,X,Y,Z order
          float quatW, quatX, quatY, quatZ;
          if (compressionLevel == 0)
          {
            memcpy(&quatW, &splatData[splatOffset + compression.rotationOffsetBytes + 0], sizeof(float));
            memcpy(&quatX, &splatData[splatOffset + compression.rotationOffsetBytes + 4], sizeof(float));
            memcpy(&quatY, &splatData[splatOffset + compression.rotationOffsetBytes + 8], sizeof(float));
            memcpy(&quatZ, &splatData[splatOffset + compression.rotationOffsetBytes + 12], sizeof(float));
          }
          else
          {
            uint16_t compressedQuatW, compressedQuatX, compressedQuatY, compressedQuatZ;
            memcpy(&compressedQuatW, &splatData[splatOffset + compression.rotationOffsetBytes + 0], sizeof(uint16_t));
            memcpy(&compressedQuatX, &splatData[splatOffset + compression.rotationOffsetBytes + 2], sizeof(uint16_t));
            memcpy(&compressedQuatY, &splatData[splatOffset + compression.rotationOffsetBytes + 4], sizeof(uint16_t));
            memcpy(&compressedQuatZ, &splatData[splatOffset + compression.rotationOffsetBytes + 6], sizeof(uint16_t));

            quatW = fromHalf(compressedQuatW);
            quatX = fromHalf(compressedQuatX);
            quatY = fromHalf(compressedQuatY);
            quatZ = fromHalf(compressedQuatZ);
          }

          // Read color (always 4 uint8 values: R, G, B, A)
          const uint8_t r = splatData[splatOffset + compression.colorOffsetBytes + 0];
          const uint8_t g = splatData[splatOffset + compression.colorOffsetBytes + 1];
          const uint8_t b = splatData[splatOffset + compression.colorOffsetBytes + 2];
          const uint8_t a = splatData[splatOffset + compression.colorOffsetBytes + 3];

          // Debug logging for first few splats to check values
          if (i < 5)
          {
            DEBUG(LOG_TAG, "Splat %d: pos=(%.3f,%.3f,%.3f) scale=(%.3f,%.3f,%.3f) quat=(%.3f,%.3f,%.3f,%.3f) color=(%.3f,%.3f,%.3f,%.3f)", i, x, y, z, scaleX, scaleY, scaleZ, quatX, quatY, quatZ, quatW, r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
          }

          // Call the splat callback with original coordinates (no conversion)
          // Unlike SPZ format, some ksplat files may already be in OpenGL coordinate system
          splatCallback(
            i,
            x,
            y,
            z,
            scaleX,
            scaleY,
            scaleZ,
            quatX,
            quatY,
            quatZ,
            quatW,
            a / 255.0f, // opacity
            r / 255.0f, // red
            g / 255.0f, // green
            b / 255.0f  // blue
          );
        }

        sectionBase += splatDataStorageSizeBytes + bucketsStorageSizeBytes;
      }

      DEBUG(LOG_TAG, "Successfully decoded KSplat file with %u splats", splatCount);
      return true;
    }
    catch (const std::exception &e)
    {
      DEBUG(LOG_TAG, "Error decoding KSplat file: %s", e.what());
      return false;
    }
  }

  bool KsplatLoader::load(const std::vector<char> &data, std::vector<builtin_scene::GaussianSplat> &splats)
  {
    splats.clear();

    return decodeKsplat(
      data,
      [&splats](int numSplats)
      {
        splats.reserve(numSplats);
      },
      [&splats](int index, float x, float y, float z, float scaleX, float scaleY, float scaleZ, float quatX, float quatY, float quatZ, float quatW, float opacity, float r, float g, float b)
      {
        builtin_scene::GaussianSplat splat;
        // Note: Coordinate conversion is already applied in the callback from decodeKsplat
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

  float KsplatLoader::fromHalf(uint16_t value)
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
  bool KsplatLoader::readBinary(const std::vector<char> &data, size_t offset, T &value)
  {
    if (offset + sizeof(T) > data.size())
    {
      return false;
    }

    std::memcpy(&value, &data[offset], sizeof(T));
    return true;
  }

  // Explicit template instantiations for the types we use
  template bool KsplatLoader::readBinary<float>(const std::vector<char> &, size_t, float &);
  template bool KsplatLoader::readBinary<uint8_t>(const std::vector<char> &, size_t, uint8_t &);
  template bool KsplatLoader::readBinary<uint16_t>(const std::vector<char> &, size_t, uint16_t &);
  template bool KsplatLoader::readBinary<uint32_t>(const std::vector<char> &, size_t, uint32_t &);
}