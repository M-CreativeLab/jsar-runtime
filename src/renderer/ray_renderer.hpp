#pragma once

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <common/collision/ray.hpp>
#include <common/classes.hpp>
#include <common/xr/input_sources.hpp>
#include <xr/device.hpp>

namespace renderer
{
  /**
   * Configuration for cursor appearance.
   */
  struct CursorConfig
  {
    // Cursor size in world units
    float size = 0.02f;
    // Cursor color (RGBA)
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    // Optional texture path for custom cursor image
    std::string texturePath;
    // Whether to show cursor at intersection points
    bool showCursor = true;
  };

  /**
   * Configuration for ray appearance.
   */
  struct RayConfig
  {
    // Ray color (RGBA)
    glm::vec4 color = glm::vec4(0.0f, 1.0f, 0.0f, 0.8f);
    // Ray width in pixels
    float width = 2.0f;
    // Maximum ray length for visualization
    float maxLength = 10.0f;
    // Whether to show rays
    bool showRay = true;
  };

  /**
   * Represents a ray and its associated cursor for rendering.
   */
  struct RayVisualization
  {
    // The collision ray
    collision::TrRay ray;
    // Input source ID that owns this ray
    int inputSourceId;
    // Ray configuration
    RayConfig rayConfig;
    // Cursor configuration
    CursorConfig cursorConfig;
    // Whether this visualization is active
    bool active = true;
  };

  /**
   * Ray renderer for visualizing input rays and collision cursors.
   * 
   * This renderer maintains multiple rays representing input/collision rays
   * and renders them along with cursors at intersection points based on
   * the scene's depth buffer.
   */
  class TrRayRenderer final
  {
  public:
    TrRayRenderer();
    ~TrRayRenderer();

  public:
    /**
     * Initialize the ray renderer with OpenGL resources.
     * Must be called from the render thread.
     */
    void initialize();

    /**
     * Shutdown and cleanup resources.
     */
    void shutdown();

    /**
     * Update ray visualizations based on current input sources.
     * 
     * @param xrDevice The XR device to fetch input sources from
     */
    void updateRays(xr::Device *xrDevice);

    /**
     * Render all active ray visualizations.
     * 
     * @param viewMatrix The current view matrix
     * @param projectionMatrix The current projection matrix
     * @param framebufferId The current framebuffer ID for depth buffer access
     * @param viewportWidth The viewport width
     * @param viewportHeight The viewport height
     */
    void render(const glm::mat4 &viewMatrix,
                const glm::mat4 &projectionMatrix,
                unsigned int framebufferId,
                int viewportWidth,
                int viewportHeight);

    /**
     * Set global ray configuration.
     * 
     * @param config The ray configuration to apply globally
     */
    void setGlobalRayConfig(const RayConfig &config);

    /**
     * Set global cursor configuration.
     * 
     * @param config The cursor configuration to apply globally
     */
    void setGlobalCursorConfig(const CursorConfig &config);

    /**
     * Set ray configuration for a specific input source.
     * 
     * @param inputSourceId The input source ID
     * @param config The ray configuration
     */
    void setRayConfig(int inputSourceId, const RayConfig &config);

    /**
     * Set cursor configuration for a specific input source.
     * 
     * @param inputSourceId The input source ID
     * @param config The cursor configuration
     */
    void setCursorConfig(int inputSourceId, const CursorConfig &config);

    /**
     * Enable or disable ray visualization globally.
     * 
     * @param enabled Whether to show rays
     */
    void setRayVisualizationEnabled(bool enabled);

    /**
     * Enable or disable cursor visualization globally.
     * 
     * @param enabled Whether to show cursors
     */
    void setCursorVisualizationEnabled(bool enabled);

  private:
    /**
     * Create OpenGL shader program for ray rendering.
     */
    void createRayShaderProgram();

    /**
     * Create OpenGL shader program for cursor rendering.
     */
    void createCursorShaderProgram();

    /**
     * Create OpenGL resources for ray geometry.
     */
    void createRayGeometry();

    /**
     * Create OpenGL resources for cursor geometry.
     */
    void createCursorGeometry();

    /**
     * Render a single ray.
     * 
     * @param rayViz The ray visualization to render
     * @param viewMatrix The view matrix
     * @param projectionMatrix The projection matrix
     */
    void renderRay(const RayVisualization &rayViz,
                   const glm::mat4 &viewMatrix,
                   const glm::mat4 &projectionMatrix);

    /**
     * Render a cursor at the ray intersection point.
     * 
     * @param rayViz The ray visualization
     * @param intersectionPoint The 3D intersection point
     * @param viewMatrix The view matrix
     * @param projectionMatrix The projection matrix
     */
    void renderCursor(const RayVisualization &rayViz,
                      const glm::vec3 &intersectionPoint,
                      const glm::mat4 &viewMatrix,
                      const glm::mat4 &projectionMatrix);

    /**
     * Calculate ray intersection with depth buffer.
     * 
     * @param ray The ray to test
     * @param viewMatrix The view matrix
     * @param projectionMatrix The projection matrix
     * @param viewportWidth The viewport width
     * @param viewportHeight The viewport height
     * @return The intersection point in world space, or nullopt if no intersection
     */
    std::optional<glm::vec3> calculateRayIntersection(
      const collision::TrRay &ray,
      const glm::mat4 &viewMatrix,
      const glm::mat4 &projectionMatrix,
      int viewportWidth,
      int viewportHeight);

    /**
     * Update or create ray visualization for an input source.
     * 
     * @param inputSource The input source
     */
    void updateRayVisualization(xr::TrXRInputSource *inputSource);

    /**
     * Remove inactive ray visualizations.
     */
    void removeInactiveRays();

  private:
    // Whether the renderer is initialized
    bool m_Initialized = false;

    // Global configuration
    RayConfig m_GlobalRayConfig;
    CursorConfig m_GlobalCursorConfig;
    bool m_RayVisualizationEnabled = true;
    bool m_CursorVisualizationEnabled = true;

    // Ray visualizations
    std::vector<RayVisualization> m_RayVisualizations;

    // OpenGL resources for rays
    unsigned int m_RayShaderProgram = 0;
    unsigned int m_RayVAO = 0;
    unsigned int m_RayVBO = 0;

    // OpenGL resources for cursors
    unsigned int m_CursorShaderProgram = 0;
    unsigned int m_CursorVAO = 0;
    unsigned int m_CursorVBO = 0;
    unsigned int m_CursorEBO = 0;

    // Shader uniform locations for rays
    int m_RayMVPUniform = -1;
    int m_RayColorUniform = -1;

    // Shader uniform locations for cursors
    int m_CursorMVPUniform = -1;
    int m_CursorColorUniform = -1;
    int m_CursorTextureUniform = -1;

    // Cursor texture (if using custom image)
    unsigned int m_CursorTexture = 0;
  };
}