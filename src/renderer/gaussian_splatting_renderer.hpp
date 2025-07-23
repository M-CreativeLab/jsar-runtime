#pragma once

#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <string>

namespace renderer
{
  /**
   * Structure representing a 3D Gaussian Splat for rendering.
   * Each splat represents an ellipsoid in 3D space with associated color and opacity.
   */
  struct GaussianSplat
  {
    // Position in 3D space (x, y, z)
    float position[3];
    
    // Rotation quaternion (x, y, z, w)
    float rotation[4];
    
    // Scale (x, y, z)
    float scale[3];
    
    // Color (r, g, b)
    float color[3];
    
    // Opacity/alpha
    float opacity;
    
    // Depth for sorting (computed during rendering)
    float depth;
  };

  /**
   * Basic 3DGS renderer that handles loading, sorting, and rendering of 3D Gaussian Splats.
   * This is a minimal implementation that provides the foundation for full 3DGS support.
   */
  class TrGaussianSplattingRenderer
  {
  public:
    TrGaussianSplattingRenderer();
    ~TrGaussianSplattingRenderer();

    /**
     * Load a 3DGS model from file.
     * @param filepath Path to the 3DGS model file (.gsplat, .ply, etc.)
     * @return true if loaded successfully, false otherwise
     */
    bool loadModel(const std::string &filepath);

    /**
     * Add gaussian splats from another model for global rendering.
     * @param splats Vector of gaussian splats to add
     */
    void addSplats(const std::vector<GaussianSplat> &splats);

    /**
     * Clear all loaded splats.
     */
    void clearSplats();

    /**
     * Get the number of loaded splats.
     * @return Number of gaussian splats
     */
    size_t getSplatCount() const;

    /**
     * Sort gaussian splats by depth for correct alpha blending.
     * This should be called from a background thread to avoid blocking rendering.
     * @param viewMatrix The current view matrix for depth calculation
     */
    void sortSplats(const float viewMatrix[16]);

    /**
     * Render all gaussian splats using WebGL2.
     * This should be called after transparent objects in the render pipeline.
     * @param projectionMatrix The projection matrix
     * @param viewMatrix The view matrix
     */
    void render(const float projectionMatrix[16], const float viewMatrix[16]);

    /**
     * Initialize WebGL2 resources for gaussian splatting.
     * @return true if initialization successful, false otherwise
     */
    bool initializeGL();

    /**
     * Clean up WebGL2 resources.
     */
    void shutdownGL();

  private:
    // Vector of all gaussian splats from all models
    std::vector<GaussianSplat> splats_;
    
    // Mutex for thread-safe access to splats
    mutable std::mutex splatsMutex_;
    
    // Flag indicating if splats need re-sorting
    std::atomic<bool> needsSorting_;
    
    // WebGL2 resources (to be implemented)
    unsigned int shaderProgram_;
    unsigned int vertexBuffer_;
    unsigned int indexBuffer_;
    
    // Helper methods
    bool loadGSplatFile(const std::string &filepath);
    bool loadPlyFile(const std::string &filepath);
    void computeDepths(const float viewMatrix[16]);
    void sortByDepth();
  };
}