#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <client/builtin_scene/gaussian_splatting.hpp>

namespace builtin_scene::model_loaders
{
  /**
   * Base class for all Gaussian Splatting model loaders.
   * Provides unified interface for both one-shot and progressive loading modes.
   * 
   * This base class enables:
   * - Progressive/chunked loading with configurable batch sizes
   * - One-shot loading for backward compatibility
   * - Consistent API across all Gaussian Splatting formats (PLY, KSplat, SPZ)
   * - State management for progressive loading sessions
   */
  class GaussianSplatLoader
  {
  public:
    /**
     * Callback function type for processing each splat during loading.
     * Parameters: index, x, y, z, scaleX, scaleY, scaleZ, quatX, quatY, quatZ, quatW, opacity, r, g, b
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
     * Progress callback for tracking loading progress.
     * Parameters: loadedCount, totalCount
     */
    using ProgressCallback = std::function<void(int loadedCount, int totalCount)>;

    virtual ~GaussianSplatLoader() = default;

    /**
     * Load all splats at once (one-shot loading).
     * @param data Raw file data
     * @param splats Output vector to store loaded splats
     * @return true if loading was successful, false otherwise
     */
    virtual bool load(const std::vector<char> &data, std::vector<builtin_scene::GaussianSplat> &splats) = 0;

    /**
     * Initialize progressive loading session.
     * Must be called before loadNextBatch().
     * @param data Raw file data
     * @param progressCallback Optional callback for progress updates
     * @return true if initialization was successful, false otherwise
     */
    virtual bool initProgressiveLoading(const std::vector<char> &data, ProgressCallback progressCallback = nullptr) = 0;

    /**
     * Load the next batch of splats in progressive mode.
     * @param batchSize Maximum number of splats to load in this batch
     * @param splats Output vector to store loaded splats (appended to existing)
     * @return true if batch was loaded successfully, false if complete or error
     */
    virtual bool loadNextBatch(size_t batchSize, std::vector<builtin_scene::GaussianSplat> &splats) = 0;

    /**
     * Check if progressive loading is complete.
     * @return true if all splats have been loaded, false otherwise
     */
    virtual bool isProgressiveLoadingComplete() const = 0;

    /**
     * Get the total number of splats available for loading.
     * Only valid after initProgressiveLoading() has been called.
     * @return Total splat count, or -1 if not initialized
     */
    virtual int getTotalSplatCount() const = 0;

    /**
     * Get the number of splats loaded so far in progressive mode.
     * @return Loaded splat count, or -1 if not in progressive mode
     */
    virtual int getLoadedSplatCount() const = 0;

    /**
     * Reset the progressive loading state.
     * Call this to restart progressive loading from the beginning.
     */
    virtual void resetProgressiveLoading() = 0;

    /**
     * Load using callback-based approach (legacy API for backward compatibility).
     * @param data Raw file data
     * @param initNumSplats Callback called with total number of splats
     * @param splatCallback Callback called for each splat
     * @return true if loading was successful, false otherwise
     */
    virtual bool loadWithCallback(
      const std::vector<char> &data,
      std::function<void(int numSplats)> initNumSplats,
      SplatCallback splatCallback) = 0;

  protected:
    /**
     * Helper function to convert splat data from callback format to GaussianSplat struct.
     * @param index Splat index
     * @param x, y, z Position coordinates
     * @param scaleX, scaleY, scaleZ Scale components
     * @param quatX, quatY, quatZ, quatW Rotation quaternion
     * @param opacity Opacity value
     * @param r, g, b Color components
     * @return GaussianSplat struct with converted data
     */
    static builtin_scene::GaussianSplat createSplat(
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
      float b);
  };
}