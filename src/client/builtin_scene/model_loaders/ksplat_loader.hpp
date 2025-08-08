#pragma once

#include <vector>
#include <string>
#include <memory>
#include "../model_3d.hpp"

namespace builtin_scene::model_loaders
{
  /**
   * Ksplat file loader for 3D Gaussian Splatting models.
   * Implements the KSplat format specification as defined in the GaussianSplats3D reference implementation:
   * https://github.com/mkkellogg/GaussianSplats3D/blob/main/src/loaders/ksplat/KSplatLoader.js
   */
  class KsplatLoader
  {
  public:
    /**
     * Load a .ksplat file from data buffer and extract Gaussian splats.
     * @param data Raw .ksplat file data
     * @param splats Output vector to store loaded splats
     * @return true if loading was successful, false otherwise
     */
    static bool load(const std::vector<char> &data, std::vector<builtin_scene::GaussianSplat> &splats);

  private:
    // KSplat format constants from the reference implementation
    static constexpr size_t HEADER_SIZE_BYTES = 4096;
    static constexpr size_t SECTION_HEADER_SIZE_BYTES = 1024;
    static constexpr size_t BUCKET_STORAGE_SIZE_FLOATS = 3;
    static constexpr size_t BUCKET_STORAGE_SIZE_BYTES = 12;

    // Compression level definitions (from SplatBuffer.CompressionLevels)
    struct CompressionLevel
    {
      size_t bytesPerCenter;
      size_t bytesPerScale;
      size_t bytesPerRotation;
      size_t bytesPerColor;
      size_t scaleOffsetBytes;
      size_t rotationOffsetBytes;
      size_t colorOffsetBytes;
      size_t sphericalHarmonicsOffsetBytes;
      uint32_t scaleRange;
      size_t bytesPerSphericalHarmonicsComponent;
      size_t sphericalHarmonicsOffsetFloat;
    };

    static const CompressionLevel COMPRESSION_LEVELS[3];

    // Header structure based on SplatBuffer.parseHeader
    struct KSplatHeader
    {
      uint8_t versionMajor;
      uint8_t versionMinor;
      uint32_t maxSectionCount;
      uint32_t sectionCount;
      uint32_t maxSplatCount;
      uint32_t splatCount;
      uint16_t compressionLevel;
      float sceneCenter[3];
      float minSphericalHarmonicsCoeff;
      float maxSphericalHarmonicsCoeff;
    };

    // Section header structure based on SplatBuffer.parseSectionHeaders
    struct KSplatSectionHeader
    {
      uint32_t splatCount;
      uint32_t maxSplatCount;
      uint32_t bucketSize;
      uint32_t bucketCount;
      float bucketBlockSize;
      uint16_t bucketStorageSizeBytes;
      uint32_t compressionScaleRange;
      uint32_t storageSizeBytes;
      uint32_t fullBucketCount;
      uint32_t partiallyFilledBucketCount;
      uint16_t sphericalHarmonicsDegree;

      // Calculated fields
      size_t bytesPerSplat;
      size_t splatCountOffset;
      size_t base;
      size_t bucketsBase;
      size_t dataBase;
      float compressionScaleFactor;
      float halfBucketBlockSize;
    };

    /**
     * Parse the KSplat file header.
     * @param data Raw file data
     * @param header Output header structure
     * @return true if header is valid and parseable
     */
    static bool parseHeader(const std::vector<char> &data, KSplatHeader &header);

    /**
     * Parse section headers from the KSplat file.
     * @param data Raw file data
     * @param header Main file header
     * @param sectionHeaders Output vector of section headers
     * @return true if section headers are valid and parseable
     */
    static bool parseSectionHeaders(const std::vector<char> &data, const KSplatHeader &header, std::vector<KSplatSectionHeader> &sectionHeaders);

    /**
     * Extract splat data from a section.
     * @param data Raw file data
     * @param header Main file header
     * @param sectionHeader Section header
     * @param splats Output vector to append extracted splats
     * @return true if splat data was successfully extracted
     */
    static bool extractSectionSplats(const std::vector<char> &data, const KSplatHeader &header, const KSplatSectionHeader &sectionHeader, std::vector<builtin_scene::GaussianSplat> &splats);

    /**
     * Read a float value from the data view based on compression level.
     * @param data Raw data buffer
     * @param offset Byte offset to read from
     * @param compressionLevel Compression level (0, 1, or 2)
     * @param isSphericalHarmonics Whether this is spherical harmonics data
     * @return The uncompressed float value
     */
    static float readCompressedFloat(const std::vector<char> &data, size_t offset, uint16_t compressionLevel, bool isSphericalHarmonics = false);

    /**
     * Convert from half-float to float (for compression level 1).
     * @param value Half-float value as uint16_t
     * @return Full float value
     */
    static float fromHalfFloat(uint16_t value);

    /**
     * Read binary data at the given offset with proper endianness handling.
     * @param data Raw file data
     * @param offset Offset in bytes
     * @param value Output value
     * @return true if read was successful, false if out of bounds
     */
    template <typename T>
    static bool readBinary(const std::vector<char> &data, size_t offset, T &value);

    /**
     * Check version compatibility.
     * @param header Parsed header
     * @return true if version is supported
     */
    static bool checkVersion(const KSplatHeader &header);
  };
}