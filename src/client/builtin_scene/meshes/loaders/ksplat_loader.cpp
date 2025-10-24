#include <cstring>
#include <algorithm>
#include <cmath>

#include "./ksplat_loader.hpp"

namespace endor
{
  namespace builtin_scene::model_loaders
  {
    using namespace std;

    static const char *LOG_TAG = "KsplatLoader";

    // Compression level definitions
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

    // SH degree to components mapping
    const unordered_map<uint16_t, uint32_t> KsplatLoader::KSPLAT_SH_DEGREE_TO_COMPONENTS = {
      {0, 0},
      {1, 9},
      {2, 24},
      {3, 45}};

    // Little-endian data reading functions (matching JavaScript DataView with littleEndian=true)
    float KsplatLoader::readFloat32LE(const uint8_t *data, size_t offset)
    {
      uint32_t value = readUint32LE(data, offset);
      return *reinterpret_cast<const float *>(&value);
    }

    uint32_t KsplatLoader::readUint32LE(const uint8_t *data, size_t offset)
    {
      return static_cast<uint32_t>(data[offset]) |
             (static_cast<uint32_t>(data[offset + 1]) << 8) |
             (static_cast<uint32_t>(data[offset + 2]) << 16) |
             (static_cast<uint32_t>(data[offset + 3]) << 24);
    }

    uint16_t KsplatLoader::readUint16LE(const uint8_t *data, size_t offset)
    {
      return static_cast<uint16_t>(data[offset]) |
             (static_cast<uint16_t>(data[offset + 1]) << 8);
    }

    uint8_t KsplatLoader::readUint8(const uint8_t *data, size_t offset)
    {
      return data[offset];
    }

    bool KsplatLoader::decodeKsplat(
      const vector<char> &fileBytes,
      function<void(int numSplats)> initNumSplats,
      SplatCallback splatCallback)
    {
      try
      {
        const size_t HEADER_BYTES = 4096;
        const size_t SECTION_BYTES = 1024;

        if (fileBytes.size() < HEADER_BYTES)
        {
          cerr << "File too small for KSplat header. Expected " << HEADER_BYTES << " bytes, got " << fileBytes.size()
               << endl;
          return false;
        }

        // Parse header
        const uint8_t *data = reinterpret_cast<const uint8_t *>(fileBytes.data());
        size_t headerOffset = 0;

        const uint8_t versionMajor = readUint8(data, 0);
        const uint8_t versionMinor = readUint8(data, 1);
        if (versionMajor != 0 || versionMinor < 1)
        {
          cerr << "Unsupported .ksplat version: "
               << static_cast<int>(versionMajor) << "." << static_cast<int>(versionMinor)
               << endl;
          return false;
        }

        const uint32_t maxSectionCount = readUint32LE(data, 4);
        // const uint32_t sectionCount = readUint32LE(data, 8);
        // const uint32_t maxSplatCount = readUint32LE(data, 12);
        const uint32_t splatCount = readUint32LE(data, 16);
        const uint16_t compressionLevel = readUint16LE(data, 20);
        // const float sceneCenterX = readFloat32LE(data, 24);
        // const float sceneCenterY = readFloat32LE(data, 28);
        // const float sceneCenterZ = readFloat32LE(data, 32);
        float minSphericalHarmonicsCoeff = readFloat32LE(data, 36);
        float maxSphericalHarmonicsCoeff = readFloat32LE(data, 40);

        // Set default SH coefficient range if not specified
        if (minSphericalHarmonicsCoeff == 0.0f)
          minSphericalHarmonicsCoeff = -1.5f;
        if (maxSphericalHarmonicsCoeff == 0.0f)
          maxSphericalHarmonicsCoeff = 1.5f;

        if (compressionLevel < 0 || compressionLevel > 2)
        {
          cerr << "Invalid .ksplat compression level: " << compressionLevel << endl;
          return false;
        }

        if (splatCount == 0)
        {
          cerr << "No splats found in KSplat file" << endl;
          return false;
        }

        // Initialize with total splat count
        initNumSplats(splatCount);

        headerOffset = HEADER_BYTES;
        size_t sectionBase = HEADER_BYTES + maxSectionCount * SECTION_BYTES;

        cout << "KSplat file: version=" << static_cast<int>(versionMajor) << "." << static_cast<int>(versionMinor)
             << ", sections=" << maxSectionCount
             << ", splats=" << splatCount
             << ", compression=" << compressionLevel
             << endl;

        // Process each section
        for (uint32_t sectionIndex = 0; sectionIndex < maxSectionCount; ++sectionIndex)
        {
          if (headerOffset + SECTION_BYTES > fileBytes.size())
          {
            cerr << "File too small for section header" << endl;
            return false;
          }

          const uint8_t *sectionData = &data[headerOffset];
          headerOffset += SECTION_BYTES;

          // Read section header using little-endian functions
          const uint32_t sectionSplatCount = readUint32LE(sectionData, 0);
          const uint32_t sectionMaxSplatCount = readUint32LE(sectionData, 4);
          const uint32_t bucketSize = readUint32LE(sectionData, 8);
          const uint32_t bucketCount = readUint32LE(sectionData, 12);
          const float bucketBlockSize = readFloat32LE(sectionData, 16);
          const uint16_t bucketStorageSizeBytes = readUint16LE(sectionData, 20);
          uint32_t compressionScaleRange = readUint32LE(sectionData, 24);
          const uint32_t fullBucketCount = readUint32LE(sectionData, 32);
          const uint32_t partiallyFilledBucketCount = readUint32LE(sectionData, 36);
          const uint16_t sphericalHarmonicsDegree = readUint16LE(sectionData, 40);

          // Use default scale range if not specified
          if (compressionScaleRange == 0)
          {
            compressionScaleRange = KSPLAT_COMPRESSION[compressionLevel].scaleRange;
          }

          // Get SH components count
          uint32_t shComponents = 0;
          auto shIt = KSPLAT_SH_DEGREE_TO_COMPONENTS.find(sphericalHarmonicsDegree);
          if (shIt != KSPLAT_SH_DEGREE_TO_COMPONENTS.end())
          {
            shComponents = shIt->second;
          }

          const auto &compression = KSPLAT_COMPRESSION[compressionLevel];

          // Calculate bytes per splat INCLUDING SH data (critical for correct offset calculation)
          const size_t bytesPerSplat = compression.bytesPerCenter +
                                       compression.bytesPerScale +
                                       compression.bytesPerRotation +
                                       compression.bytesPerColor +
                                       shComponents * compression.bytesPerSphericalHarmonicsComponent;

          const size_t bucketsMetaDataSizeBytes = partiallyFilledBucketCount * 4;
          const size_t bucketsStorageSizeBytes = bucketStorageSizeBytes * bucketCount + bucketsMetaDataSizeBytes;
          const size_t splatDataStorageSizeBytes = bytesPerSplat * sectionMaxSplatCount;
          const size_t storageSizeBytes = splatDataStorageSizeBytes + bucketsStorageSizeBytes;

          if (sectionSplatCount == 0)
          {
            sectionBase += storageSizeBytes;
            continue;
          }

          const float compressionScaleFactor = bucketBlockSize / 2.0f / static_cast<float>(compressionScaleRange);
          const size_t bucketsBase = sectionBase + bucketsMetaDataSizeBytes;
          const size_t dataBase = sectionBase + bucketsStorageSizeBytes;

          if (dataBase + splatDataStorageSizeBytes > fileBytes.size())
          {
            cerr << "File too small for section data. Expected " << dataBase + splatDataStorageSizeBytes
                 << ", available " << fileBytes.size()
                 << endl;
            return false;
          }

          const uint8_t *splatData = &data[dataBase];
          const uint8_t *bucketData = &data[bucketsBase];
          const uint8_t *partiallyFilledBucketLengthsData = &data[sectionBase];

          const uint32_t fullBucketSplats = fullBucketCount * bucketSize;
          uint32_t partialBucketIndex = fullBucketCount;
          uint32_t partialBucketBase = fullBucketSplats;

          // Debug first bucket data
          if (bucketCount > 0 && sectionIndex == 0)
          {
            float bucket0X = readFloat32LE(bucketData, 0);
            float bucket0Y = readFloat32LE(bucketData, 4);
            float bucket0Z = readFloat32LE(bucketData, 8);
          }

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
              const uint32_t bucketLength = readUint32LE(partiallyFilledBucketLengthsData, (partialBucketIndex - fullBucketCount) * 4);
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
              x = readFloat32LE(splatData, splatOffset + 0);
              y = readFloat32LE(splatData, splatOffset + 4);
              z = readFloat32LE(splatData, splatOffset + 8);
            }
            else
            {
              const uint16_t compressedX = readUint16LE(splatData, splatOffset + 0);
              const uint16_t compressedY = readUint16LE(splatData, splatOffset + 2);
              const uint16_t compressedZ = readUint16LE(splatData, splatOffset + 4);

              const float bucketCenterX = readFloat32LE(bucketData, bucketIndex * 3 * 4 + 0);
              const float bucketCenterY = readFloat32LE(bucketData, bucketIndex * 3 * 4 + 4);
              const float bucketCenterZ = readFloat32LE(bucketData, bucketIndex * 3 * 4 + 8);

              x = (static_cast<float>(compressedX) - static_cast<float>(compressionScaleRange)) * compressionScaleFactor + bucketCenterX;
              y = (static_cast<float>(compressedY) - static_cast<float>(compressionScaleRange)) * compressionScaleFactor + bucketCenterY;
              z = (static_cast<float>(compressedZ) - static_cast<float>(compressionScaleRange)) * compressionScaleFactor + bucketCenterZ;
            }

            // Read scale
            float scaleX, scaleY, scaleZ;
            if (compressionLevel == 0)
            {
              scaleX = readFloat32LE(splatData, splatOffset + compression.scaleOffsetBytes + 0);
              scaleY = readFloat32LE(splatData, splatOffset + compression.scaleOffsetBytes + 4);
              scaleZ = readFloat32LE(splatData, splatOffset + compression.scaleOffsetBytes + 8);
            }
            else
            {
              const uint16_t compressedScaleX = readUint16LE(splatData, splatOffset + compression.scaleOffsetBytes + 0);
              const uint16_t compressedScaleY = readUint16LE(splatData, splatOffset + compression.scaleOffsetBytes + 2);
              const uint16_t compressedScaleZ = readUint16LE(splatData, splatOffset + compression.scaleOffsetBytes + 4);

              scaleX = fromHalf(compressedScaleX);
              scaleY = fromHalf(compressedScaleY);
              scaleZ = fromHalf(compressedScaleZ);
            }

            // Read rotation (quaternion)
            float quatW, quatX, quatY, quatZ;
            if (compressionLevel == 0)
            {
              quatW = readFloat32LE(splatData, splatOffset + compression.rotationOffsetBytes + 0);
              quatX = readFloat32LE(splatData, splatOffset + compression.rotationOffsetBytes + 4);
              quatY = readFloat32LE(splatData, splatOffset + compression.rotationOffsetBytes + 8);
              quatZ = readFloat32LE(splatData, splatOffset + compression.rotationOffsetBytes + 12);
            }
            else
            {
              const uint16_t compressedQuatW = readUint16LE(splatData, splatOffset + compression.rotationOffsetBytes + 0);
              const uint16_t compressedQuatX = readUint16LE(splatData, splatOffset + compression.rotationOffsetBytes + 2);
              const uint16_t compressedQuatY = readUint16LE(splatData, splatOffset + compression.rotationOffsetBytes + 4);
              const uint16_t compressedQuatZ = readUint16LE(splatData, splatOffset + compression.rotationOffsetBytes + 6);

              quatW = fromHalf(compressedQuatW);
              quatX = fromHalf(compressedQuatX);
              quatY = fromHalf(compressedQuatY);
              quatZ = fromHalf(compressedQuatZ);
            }

            // Normalize quaternion (critical fix!)
            float quatLength = sqrt(quatW * quatW + quatX * quatX + quatY * quatY + quatZ * quatZ);
            if (quatLength > 0.0f)
            {
              quatW /= quatLength;
              quatX /= quatLength;
              quatY /= quatLength;
              quatZ /= quatLength;
            }
            else
            {
              // Default unit quaternion
              quatW = 1.0f;
              quatX = quatY = quatZ = 0.0f;
            }

            // Read color (always 4 uint8 values: R, G, B, A)
            const uint8_t r = readUint8(splatData, splatOffset + compression.colorOffsetBytes + 0);
            const uint8_t g = readUint8(splatData, splatOffset + compression.colorOffsetBytes + 1);
            const uint8_t b = readUint8(splatData, splatOffset + compression.colorOffsetBytes + 2);
            const uint8_t opacity = readUint8(splatData, splatOffset + compression.colorOffsetBytes + 3);

            // Call the splat callback with normalized quaternion
            // Convert the space from COLMAP to OPENGL
            splatCallback(
              i,
              x,
              -y,
              -z,
              scaleX,
              scaleY,
              scaleZ,
              quatX,
              -quatY,
              -quatZ,
              quatW,
              opacity / 255.0f, // opacity
              r / 255.0f,       // red
              g / 255.0f,       // green
              b / 255.0f        // blue
            );
          }

          sectionBase += storageSizeBytes;
        }

        return true;
      }
      catch (const exception &e)
      {
        cerr << "Error decoding KSplat file: " << e.what() << endl;
        return false;
      }
    }

    bool KsplatLoader::load(const vector<char> &data, vector<builtin_scene::GaussianSplat> &splats)
    {
      splats.clear();

      return decodeKsplat(
        data,
        [&splats](int numSplats)
        {
          splats.reserve(numSplats);
        },
        [&splats](int index,
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
    bool KsplatLoader::readBinary(const vector<char> &data, size_t offset, T &value)
    {
      if (offset + sizeof(T) > data.size())
      {
        return false;
      }

      memcpy(&value, &data[offset], sizeof(T));
      return true;
    }

    // Explicit template instantiations for the types we use
    template bool KsplatLoader::readBinary<float>(const vector<char> &, size_t, float &);
    template bool KsplatLoader::readBinary<uint8_t>(const vector<char> &, size_t, uint8_t &);
    template bool KsplatLoader::readBinary<uint16_t>(const vector<char> &, size_t, uint16_t &);
    template bool KsplatLoader::readBinary<uint32_t>(const vector<char> &, size_t, uint32_t &);
  }
} // namespace endor
