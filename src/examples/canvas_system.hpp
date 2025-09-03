#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <OpenGL/gl3.h>
#include <glm/glm.hpp>

#include <include/core/SkCanvas.h>
#include <include/core/SkSurface.h>
#include <include/core/SkImageInfo.h>

#include "./screen_renderer.hpp"

namespace jsar::example
{
  class Content;

  /**
   * Event types for ray interactions
   */
  enum class RayEventType
  {
    Move,
    Down,
    Up
  };

  /**
   * Ray event data structure
   */
  struct RayEvent
  {
    RayEventType type;
    glm::vec3 rayOrigin;
    glm::vec3 rayDirection;
    glm::vec2 localPosition; // Position on the canvas (normalized 0-1)
  };

  /**
   * Canvas system using Skia for GUI drawing.
   * Can listen to ray events (move, down, up) and can be rendered
   * on screen via ScreenRenderer or on 3D mesh via MeshRenderer.
   */
  class CanvasSystem : public ScreenComponent
  {
  public:
    using RayEventHandler = std::function<bool(const RayEvent &event)>;

    CanvasSystem(int width = 512, int height = 128);
    ~CanvasSystem();

    /**
     * Register a handler for ray events
     */
    void registerRayEventHandler(RayEventHandler handler);

    /**
     * Process a ray event (move, down, up)
     */
    bool processRayEvent(const RayEvent &event);

    /**
     * Get the generated texture for use in MeshRenderer
     */
    GLuint getTexture() const
    {
      return texture_;
    }

    /**
     * Get texture dimensions
     */
    void getTextureDimensions(int &width, int &height) const;

    /**
     * Update content state for visual feedback
     */
    void updateContentState(Content *content, bool isHovered, bool isDragging);

    /**
     * Regenerate the texture with current canvas content
     */
    void updateTexture();

    // ScreenComponent interface for ScreenRenderer
    void render() override;
    void handleMouseButton(GLFWwindow *window, int button, int action, double xpos, double ypos) override;

  protected:
    /**
     * Draw the GUI content using Skia Canvas.
     * Override this method to customize the canvas content.
     */
    virtual void drawCanvas(SkCanvas *canvas);

  private:
    void initializeSkiaSurface();
    void createOpenGLTexture();
    void renderAppleStyleBar(SkCanvas *canvas);

  private:
    int width_;
    int height_;
    GLuint texture_;

    // Skia surface for drawing
    sk_sp<SkSurface> surface_;
    std::vector<uint8_t> pixelBuffer_;

    // Event handling
    std::vector<RayEventHandler> rayEventHandlers_;

    // Content state tracking
    struct ContentState
    {
      Content *content;
      bool isHovered;
      bool isDragging;

      ContentState(Content *c)
          : content(c)
          , isHovered(false)
          , isDragging(false)
      {
      }
    };
    std::vector<ContentState> contentStates_;
  };
}