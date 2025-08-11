#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>
#include <client/builtin_scene/gaussian_splatting.hpp>

namespace builtin_scene::model_loaders
{
  /**
   * Ksplat file loader for 3D Gaussian Splatting models.
   * Follows the Spark implementation pattern with correct endianness handling:
   * https://github.com/sparkjsdev/spark/blob/main/src/ksplat.ts
   */
  class KsplatLoader
  {
  public:
    /**
     * Callback function type for processing each splat during loading.
     * Parameters: index, x, y, z, scaleX, scaleY, scaleZ, quatX, quatY, quatZ, quatW, opacity, r, g, b
     *
     * Note: the returned values are in the OpenGL coordinate system (Y-up).
     */
    using SplatCallback = std::function<void(
      int index,
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
      float b)>;

    /**
     * Decode .ksplat file from data buffer using callback-based approach.
     * @param fileBytes Raw .ksplat file data
     * @param initNumSplats Callback called with total number of splats
     * @param splatCallback Callback called for each splat
     * @return true if loading was successful, false otherwise
     */
    static bool decodeKsplat(
      const std::vector<char> &fileBytes,
      std::function<void(int numSplats)> initNumSplats,
      SplatCallback splatCallback);

    /**
     * Convenience method to load splats into a vector (for backward compatibility).
     * @param data Raw .ksplat file data
     * @param splats Output vector to store loaded splats
     * @return true if loading was successful, false otherwise
     */
    static bool load(const std::vector<char> &data, std::vector<builtin_scene::GaussianSplat> &splats);

  private:
    // Compression level definitions
    struct KsplatCompression
    {
      int bytesPerCenter;
      int bytesPerScale;
      int bytesPerRotation;
      int bytesPerColor;
      int bytesPerSphericalHarmonicsComponent;
      int scaleOffsetBytes;
      int rotationOffsetBytes;
      int colorOffsetBytes;
      int sphericalHarmonicsOffsetBytes;
      int scaleRange;
    };

    static const KsplatCompression KSPLAT_COMPRESSION[3];
    static const std::unordered_map<uint16_t, uint32_t> KSPLAT_SH_DEGREE_TO_COMPONENTS;

    // Little-endian data reading functions (matching JavaScript DataView with littleEndian=true)
    static float readFloat32LE(const uint8_t *data, size_t offset);
    static uint32_t readUint32LE(const uint8_t *data, size_t offset);
    static uint16_t readUint16LE(const uint8_t *data, size_t offset);
    static uint8_t readUint8(const uint8_t *data, size_t offset);

    /**
     * Convert from half-float to float (for compression level 1).
     * @param value Half-float value as uint16_t
     * @return Full float value
     */
    static float fromHalf(uint16_t value);

    /**
     * Read binary data at the given offset with proper endianness handling.
     * @param data Raw file data
     * @param offset Offset in bytes
     * @param value Output value
     * @return true if read was successful, false if out of bounds
     */
    template <typename T>
    static bool readBinary(const std::vector<char> &data, size_t offset, T &value);
  };
}
