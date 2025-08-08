#include "./ksplat_loader.hpp"
#include <common/debug.hpp>
#include <cstring>
#include <algorithm>
#include <cmath>

namespace builtin_scene::model_loaders
{
  static const char *LOG_TAG = "KsplatLoader";

  // Compression level definitions from SplatBuffer.CompressionLevels
  const KsplatLoader::CompressionLevel KsplatLoader::COMPRESSION_LEVELS[3] = {
    // Level 0: Uncompressed
    {
      .bytesPerCenter = 12,
      .bytesPerScale = 12,
      .bytesPerRotation = 16,
      .bytesPerColor = 4,
      .scaleOffsetBytes = 12,
      .rotationOffsetBytes = 24,
      .colorOffsetBytes = 40,
      .sphericalHarmonicsOffsetBytes = 44,
      .scaleRange = 1,
      .bytesPerSphericalHarmonicsComponent = 4,
      .sphericalHarmonicsOffsetFloat = 11},
    // Level 1: Half-float compression
    {
      .bytesPerCenter = 6,
      .bytesPerScale = 6,
      .bytesPerRotation = 8,
      .bytesPerColor = 4,
      .scaleOffsetBytes = 6,
      .rotationOffsetBytes = 12,
      .colorOffsetBytes = 20,
      .sphericalHarmonicsOffsetBytes = 24,
      .scaleRange = 32767,
      .bytesPerSphericalHarmonicsComponent = 2,
      .sphericalHarmonicsOffsetFloat = 12},
    // Level 2: 8-bit compression
    {
      .bytesPerCenter = 6,
      .bytesPerScale = 6,
      .bytesPerRotation = 8,
      .bytesPerColor = 4,
      .scaleOffsetBytes = 6,
      .rotationOffsetBytes = 12,
      .colorOffsetBytes = 20,
      .sphericalHarmonicsOffsetBytes = 24,
      .scaleRange = 32767,
      .bytesPerSphericalHarmonicsComponent = 1,
      .sphericalHarmonicsOffsetFloat = 12}};

  bool KsplatLoader::load(const std::vector<char> &data, std::vector<builtin_scene::GaussianSplat> &splats)
  {
    try
    {
      // Parse the main header
      KSplatHeader header;
      if (!parseHeader(data, header))
      {
        DEBUG(LOG_TAG, "Failed to parse KSplat header");
        return false;
      }

      if (!checkVersion(header))
      {
        DEBUG(LOG_TAG, "Unsupported KSplat version: v%d.%d", header.versionMajor, header.versionMinor);
        return false;
      }

      if (header.splatCount == 0)
      {
        DEBUG(LOG_TAG, "No splats found in KSplat file");
        return false;
      }

      // Parse section headers
      std::vector<KSplatSectionHeader> sectionHeaders;
      if (!parseSectionHeaders(data, header, sectionHeaders))
      {
        DEBUG(LOG_TAG, "Failed to parse KSplat section headers");
        return false;
      }

      // Clear output and reserve space
      splats.clear();
      splats.reserve(header.splatCount);

      // Extract splats from each section
      for (const auto &sectionHeader : sectionHeaders)
      {
        if (!extractSectionSplats(data, header, sectionHeader, splats))
        {
          DEBUG(LOG_TAG, "Failed to extract splats from section");
          return false;
        }
      }

      // Sort splats by depth (z-coordinate) for proper transparency blending
      std::sort(splats.begin(), splats.end(), [](const builtin_scene::GaussianSplat &a, const builtin_scene::GaussianSplat &b)
                {
                  return a.position[2] < b.position[2]; // Sort by z-coordinate (depth)
                });

      DEBUG(LOG_TAG, "Successfully loaded KSplat file with %zu splats (sorted by depth)", splats.size());
      return true;
    }
    catch (const std::exception &e)
    {
      DEBUG(LOG_TAG, "Error loading KSplat file: %s", e.what());
      return false;
    }
  }

  bool KsplatLoader::parseHeader(const std::vector<char> &data, KSplatHeader &header)
  {
    if (data.size() < HEADER_SIZE_BYTES)
    {
      DEBUG(LOG_TAG, "File too small for KSplat header. Expected %zu bytes, got %zu", HEADER_SIZE_BYTES, data.size());
      return false;
    }

    // Parse header following SplatBuffer.parseHeader structure
    const uint8_t *headerArrayUint8 = reinterpret_cast<const uint8_t *>(data.data());
    const uint16_t *headerArrayUint16 = reinterpret_cast<const uint16_t *>(data.data());
    const uint32_t *headerArrayUint32 = reinterpret_cast<const uint32_t *>(data.data());
    const float *headerArrayFloat32 = reinterpret_cast<const float *>(data.data());

    header.versionMajor = headerArrayUint8[0];
    header.versionMinor = headerArrayUint8[1];
    header.maxSectionCount = headerArrayUint32[1];
    header.sectionCount = headerArrayUint32[2];
    header.maxSplatCount = headerArrayUint32[3];
    header.splatCount = headerArrayUint32[4];
    header.compressionLevel = headerArrayUint16[10];

    header.sceneCenter[0] = headerArrayFloat32[6];
    header.sceneCenter[1] = headerArrayFloat32[7];
    header.sceneCenter[2] = headerArrayFloat32[8];

    // Default spherical harmonics coefficients if not present
    header.minSphericalHarmonicsCoeff = headerArrayFloat32[9] != 0.0f ? headerArrayFloat32[9] : -7.5f;
    header.maxSphericalHarmonicsCoeff = headerArrayFloat32[10] != 0.0f ? headerArrayFloat32[10] : 7.5f;

    return true;
  }

  bool KsplatLoader::parseSectionHeaders(const std::vector<char> &data, const KSplatHeader &header, std::vector<KSplatSectionHeader> &sectionHeaders)
  {
    const size_t sectionHeadersOffset = HEADER_SIZE_BYTES;
    const size_t sectionHeadersSize = header.maxSectionCount * SECTION_HEADER_SIZE_BYTES;

    if (data.size() < sectionHeadersOffset + sectionHeadersSize)
    {
      DEBUG(LOG_TAG, "File too small for section headers. Expected %zu bytes, got %zu", sectionHeadersOffset + sectionHeadersSize, data.size());
      return false;
    }

    sectionHeaders.clear();
    sectionHeaders.reserve(header.maxSectionCount);

    const uint16_t *sectionHeaderArrayUint16 = reinterpret_cast<const uint16_t *>(data.data() + sectionHeadersOffset);
    const uint32_t *sectionHeaderArrayUint32 = reinterpret_cast<const uint32_t *>(data.data() + sectionHeadersOffset);
    const float *sectionHeaderArrayFloat32 = reinterpret_cast<const float *>(data.data() + sectionHeadersOffset);

    size_t sectionBase = HEADER_SIZE_BYTES + header.maxSectionCount * SECTION_HEADER_SIZE_BYTES;
    size_t splatCountOffset = 0;

    for (uint32_t i = 0; i < header.maxSectionCount; i++)
    {
      const size_t sectionHeaderBase = i * SECTION_HEADER_SIZE_BYTES;
      const size_t sectionHeaderBaseUint16 = sectionHeaderBase / 2;
      const size_t sectionHeaderBaseUint32 = sectionHeaderBase / 4;

      KSplatSectionHeader sectionHeader;

      sectionHeader.splatCount = sectionHeaderArrayUint32[sectionHeaderBaseUint32];
      sectionHeader.maxSplatCount = sectionHeaderArrayUint32[sectionHeaderBaseUint32 + 1];
      sectionHeader.bucketSize = sectionHeaderArrayUint32[sectionHeaderBaseUint32 + 2];
      sectionHeader.bucketCount = sectionHeaderArrayUint32[sectionHeaderBaseUint32 + 3];
      sectionHeader.bucketBlockSize = sectionHeaderArrayFloat32[sectionHeaderBaseUint32 + 4];
      sectionHeader.bucketStorageSizeBytes = sectionHeaderArrayUint16[sectionHeaderBaseUint16 + 10];
      sectionHeader.compressionScaleRange = sectionHeaderArrayUint32[sectionHeaderBaseUint32 + 6];
      sectionHeader.storageSizeBytes = sectionHeaderArrayUint32[sectionHeaderBaseUint32 + 7];
      sectionHeader.fullBucketCount = sectionHeaderArrayUint32[sectionHeaderBaseUint32 + 8];
      sectionHeader.partiallyFilledBucketCount = sectionHeaderArrayUint32[sectionHeaderBaseUint32 + 9];
      sectionHeader.sphericalHarmonicsDegree = sectionHeaderArrayUint16[sectionHeaderBaseUint16 + 20];

      // Calculate derived fields
      const CompressionLevel &compressionInfo = COMPRESSION_LEVELS[header.compressionLevel];

      // For simplicity, assume degree 0 spherical harmonics (no SH)
      const size_t sphericalHarmonicsComponentsPerSplat = 0; // Degree 0 = no SH components
      const size_t sphericalHarmonicsBytesPerSplat = compressionInfo.bytesPerSphericalHarmonicsComponent * sphericalHarmonicsComponentsPerSplat;

      sectionHeader.bytesPerSplat = compressionInfo.bytesPerCenter +
                                    compressionInfo.bytesPerScale +
                                    compressionInfo.bytesPerRotation +
                                    compressionInfo.bytesPerColor +
                                    sphericalHarmonicsBytesPerSplat;

      sectionHeader.splatCountOffset = splatCountOffset;
      sectionHeader.halfBucketBlockSize = sectionHeader.bucketBlockSize / 2.0f;

      if (header.compressionLevel >= 1 && sectionHeader.compressionScaleRange > 0)
      {
        sectionHeader.compressionScaleFactor = sectionHeader.halfBucketBlockSize / (float)sectionHeader.compressionScaleRange;
      }
      else
      {
        sectionHeader.compressionScaleFactor = 1.0f;
      }

      const size_t bucketsMetaDataSizeBytes = sectionHeader.partiallyFilledBucketCount * 4;
      const size_t bucketsStorageSizeBytes = sectionHeader.bucketStorageSizeBytes * sectionHeader.bucketCount + bucketsMetaDataSizeBytes;

      sectionHeader.base = sectionBase;
      sectionHeader.bucketsBase = sectionBase + bucketsMetaDataSizeBytes;
      sectionHeader.dataBase = sectionBase + bucketsStorageSizeBytes;

      sectionHeaders.push_back(sectionHeader);

      sectionBase += sectionHeader.storageSizeBytes;
      splatCountOffset += sectionHeader.maxSplatCount;
    }

    return true;
  }

  bool KsplatLoader::extractSectionSplats(const std::vector<char> &data, const KSplatHeader &header, const KSplatSectionHeader &sectionHeader, std::vector<builtin_scene::GaussianSplat> &splats)
  {
    if (sectionHeader.splatCount == 0)
    {
      return true; // Empty section is valid
    }

    const size_t sectionDataEnd = sectionHeader.dataBase + sectionHeader.splatCount * sectionHeader.bytesPerSplat;
    if (data.size() < sectionDataEnd)
    {
      DEBUG(LOG_TAG, "File too small for section data. Expected %zu bytes, got %zu", sectionDataEnd, data.size());
      return false;
    }

    const CompressionLevel &compressionInfo = COMPRESSION_LEVELS[header.compressionLevel];

    // For compression level >= 1, we need bucket information
    const float *bucketArray = nullptr;
    const uint32_t *partiallyFilledBucketLengths = nullptr;

    if (header.compressionLevel >= 1)
    {
      bucketArray = reinterpret_cast<const float *>(data.data() + sectionHeader.bucketsBase);
      if (sectionHeader.partiallyFilledBucketCount > 0)
      {
        partiallyFilledBucketLengths = reinterpret_cast<const uint32_t *>(data.data() + sectionHeader.base);
      }
    }

    // Extract splats
    for (uint32_t localSplatIndex = 0; localSplatIndex < sectionHeader.splatCount; localSplatIndex++)
    {
      builtin_scene::GaussianSplat splat;

      const size_t splatOffset = sectionHeader.dataBase + localSplatIndex * sectionHeader.bytesPerSplat;

      // Read position (center)
      if (header.compressionLevel == 0)
      {
        // Uncompressed: direct float values
        if (!readBinary(data, splatOffset, splat.position[0]) ||
            !readBinary(data, splatOffset + 4, splat.position[1]) ||
            !readBinary(data, splatOffset + 8, splat.position[2]))
        {
          DEBUG(LOG_TAG, "Failed to read position for splat %u", localSplatIndex);
          return false;
        }
      }
      else
      {
        // Compressed: need to decompress relative to bucket center
        uint16_t compressedX, compressedY, compressedZ;
        if (!readBinary(data, splatOffset, compressedX) ||
            !readBinary(data, splatOffset + 2, compressedY) ||
            !readBinary(data, splatOffset + 4, compressedZ))
        {
          DEBUG(LOG_TAG, "Failed to read compressed position for splat %u", localSplatIndex);
          return false;
        }

        // Find bucket for this splat
        uint32_t bucketIndex = 0;
        const uint32_t maxSplatIndexInFullBuckets = sectionHeader.fullBucketCount * sectionHeader.bucketSize;

        if (localSplatIndex < maxSplatIndexInFullBuckets)
        {
          bucketIndex = localSplatIndex / sectionHeader.bucketSize;
        }
        else
        {
          // Handle partially filled buckets
          uint32_t bucketSplatIndex = maxSplatIndexInFullBuckets;
          bucketIndex = sectionHeader.fullBucketCount;
          uint32_t partiallyFullBucketIndex = 0;

          while (bucketSplatIndex < sectionHeader.splatCount)
          {
            uint32_t currentPartiallyFilledBucketSize = partiallyFilledBucketLengths[partiallyFullBucketIndex];
            if (localSplatIndex >= bucketSplatIndex && localSplatIndex < bucketSplatIndex + currentPartiallyFilledBucketSize)
            {
              break;
            }
            bucketSplatIndex += currentPartiallyFilledBucketSize;
            bucketIndex++;
            partiallyFullBucketIndex++;
          }
        }

        // Decompress position relative to bucket center
        const float bucketCenterX = bucketArray[bucketIndex * 3];
        const float bucketCenterY = bucketArray[bucketIndex * 3 + 1];
        const float bucketCenterZ = bucketArray[bucketIndex * 3 + 2];

        const float sf = sectionHeader.compressionScaleFactor;
        const float sr = (float)sectionHeader.compressionScaleRange;

        splat.position[0] = (compressedX - sr) * sf + bucketCenterX;
        splat.position[1] = (compressedY - sr) * sf + bucketCenterY;
        splat.position[2] = (compressedZ - sr) * sf + bucketCenterZ;
      }

      // Read scale
      const size_t scaleOffset = splatOffset + compressionInfo.scaleOffsetBytes;
      if (header.compressionLevel == 0)
      {
        float scaleValues[3];
        if (!readBinary(data, scaleOffset, scaleValues[0]) ||
            !readBinary(data, scaleOffset + 4, scaleValues[1]) ||
            !readBinary(data, scaleOffset + 8, scaleValues[2]))
        {
          DEBUG(LOG_TAG, "Failed to read scale for splat %u", localSplatIndex);
          return false;
        }
        splat.scale[0] = scaleValues[0];
        splat.scale[1] = scaleValues[1];
        splat.scale[2] = scaleValues[2];
      }
      else
      {
        uint16_t scaleValues[3];
        if (!readBinary(data, scaleOffset, scaleValues[0]) ||
            !readBinary(data, scaleOffset + 2, scaleValues[1]) ||
            !readBinary(data, scaleOffset + 4, scaleValues[2]))
        {
          DEBUG(LOG_TAG, "Failed to read compressed scale for splat %u", localSplatIndex);
          return false;
        }
        splat.scale[0] = fromHalfFloat(scaleValues[0]);
        splat.scale[1] = fromHalfFloat(scaleValues[1]);
        splat.scale[2] = fromHalfFloat(scaleValues[2]);
      }

      // Read rotation (quaternion: x, y, z, w)
      const size_t rotationOffset = splatOffset + compressionInfo.rotationOffsetBytes;
      if (header.compressionLevel == 0)
      {
        float rotationValues[4];
        if (!readBinary(data, rotationOffset, rotationValues[0]) ||
            !readBinary(data, rotationOffset + 4, rotationValues[1]) ||
            !readBinary(data, rotationOffset + 8, rotationValues[2]) ||
            !readBinary(data, rotationOffset + 12, rotationValues[3]))
        {
          DEBUG(LOG_TAG, "Failed to read rotation for splat %u", localSplatIndex);
          return false;
        }
        splat.rotation[0] = rotationValues[0]; // x
        splat.rotation[1] = rotationValues[1]; // y
        splat.rotation[2] = rotationValues[2]; // z
        splat.rotation[3] = rotationValues[3]; // w
      }
      else
      {
        uint16_t rotationValues[4];
        if (!readBinary(data, rotationOffset, rotationValues[0]) ||
            !readBinary(data, rotationOffset + 2, rotationValues[1]) ||
            !readBinary(data, rotationOffset + 4, rotationValues[2]) ||
            !readBinary(data, rotationOffset + 6, rotationValues[3]))
        {
          DEBUG(LOG_TAG, "Failed to read compressed rotation for splat %u", localSplatIndex);
          return false;
        }
        splat.rotation[0] = fromHalfFloat(rotationValues[0]); // x
        splat.rotation[1] = fromHalfFloat(rotationValues[1]); // y
        splat.rotation[2] = fromHalfFloat(rotationValues[2]); // z
        splat.rotation[3] = fromHalfFloat(rotationValues[3]); // w
      }

      // Read color (always 4 uint8 values: R, G, B, A)
      const size_t colorOffset = splatOffset + compressionInfo.colorOffsetBytes;
      uint8_t colorValues[4];
      if (!readBinary(data, colorOffset, colorValues[0]) ||
          !readBinary(data, colorOffset + 1, colorValues[1]) ||
          !readBinary(data, colorOffset + 2, colorValues[2]) ||
          !readBinary(data, colorOffset + 3, colorValues[3]))
      {
        DEBUG(LOG_TAG, "Failed to read color for splat %u", localSplatIndex);
        return false;
      }

      // Convert from 0-255 to 0.0-1.0 range
      splat.color[0] = colorValues[0] / 255.0f;
      splat.color[1] = colorValues[1] / 255.0f;
      splat.color[2] = colorValues[2] / 255.0f;
      splat.opacity = colorValues[3] / 255.0f;

      splats.push_back(splat);
    }

    return true;
  }

  float KsplatLoader::fromHalfFloat(uint16_t value)
  {
    // Simple half-float to float conversion
    // This is a basic implementation; a proper implementation would use IEEE 754 conversion
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

  bool KsplatLoader::checkVersion(const KSplatHeader &header)
  {
    // Support version 0.1 and later (as per the JavaScript reference)
    const uint8_t minVersionMajor = 0;
    const uint8_t minVersionMinor = 1;

    if (header.versionMajor == minVersionMajor && header.versionMinor >= minVersionMinor)
    {
      return true;
    }
    else if (header.versionMajor > minVersionMajor)
    {
      return true;
    }

    return false;
  }

  // Explicit template instantiations for the types we use
  template bool KsplatLoader::readBinary<float>(const std::vector<char> &, size_t, float &);
  template bool KsplatLoader::readBinary<uint8_t>(const std::vector<char> &, size_t, uint8_t &);
  template bool KsplatLoader::readBinary<uint16_t>(const std::vector<char> &, size_t, uint16_t &);
  template bool KsplatLoader::readBinary<uint32_t>(const std::vector<char> &, size_t, uint32_t &);
}