#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <client/builtin_scene/gaussian_splatting.hpp>
#include "./gaussian_splat_loader.hpp"

namespace builtin_scene::model_loaders
{
  /**
   * SPZ file loader for 3D Gaussian Splatting models.
   * Follows the Spark implementation pattern:
   * https://github.com/sparkjsdev/spark/blob/main/src/spz.ts
   * 
   * SPZ is a compressed format (gzip) with fixed-point encoding optimized for file size.
   */
  class SpzLoader : public GaussianSplatLoader
  {
  public:
    /**
     * Decode .spz file from data buffer using callback-based approach.
     * @param fileBytes Raw .spz file data (gzip compressed)
     * @param initNumSplats Callback called with total number of splats
     * @param splatCallback Callback called for each splat
     * @return true if loading was successful, false otherwise
     */
    static bool decodeSpz(
      const std::vector<char> &fileBytes,
      std::function<void(int numSplats)> initNumSplats,
      SplatCallback splatCallback);

    // Override base class methods for progressive loading
    bool load(const std::vector<char> &data, std::vector<builtin_scene::GaussianSplat> &splats) override;
    bool initProgressiveLoading(const std::vector<char> &data, ProgressCallback progressCallback = nullptr) override;
    bool loadNextBatch(size_t batchSize, std::vector<builtin_scene::GaussianSplat> &splats) override;
    bool isProgressiveLoadingComplete() const override;
    int getTotalSplatCount() const override;
    int getLoadedSplatCount() const override;
    void resetProgressiveLoading() override;
    bool loadWithCallback(
      const std::vector<char> &data,
      std::function<void(int numSplats)> initNumSplats,
      SplatCallback splatCallback) override;

    /**
     * Convenience method to load splats into a vector (for backward compatibility).
     * @param data Raw .spz file data
     * @param splats Output vector to store loaded splats
     * @return true if loading was successful, false otherwise
     */
    static bool load(const std::vector<char> &data, std::vector<builtin_scene::GaussianSplat> &splats);

  private:
    // SPZ format constants following Spark implementation
    static constexpr uint32_t SPZ_MAGIC = 0x5053474e; // NGSP = Niantic gaussian splat
    static constexpr float SH_C0 = 0.28209479177387814f;

    /**
     * Simple gzip decompression using zlib.
     * @param compressedData Gzip compressed data
     * @param decompressedData Output decompressed data
     * @return true if decompression was successful
     */
    static bool decompressGzip(const std::vector<char> &compressedData, std::vector<char> &decompressedData);

    /**
     * Convert from half-float to float (for SPZ version 1).
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

  private:
    // Progressive loading state
    std::vector<char> progressiveData_;
    std::vector<char> decompressedData_;
    ProgressCallback progressCallback_;
    int totalSplats_ = -1;
    int loadedSplats_ = 0;
    bool progressiveInitialized_ = false;

    // Parsed SPZ structure for progressive loading
    uint32_t version_ = 0;
    size_t headerSize_ = 0;
  };
}