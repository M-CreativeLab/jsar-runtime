#pragma once

#include <memory>
#include <glm/glm.hpp>

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkPaint.h>
#include <common/font/cache.hpp>

#include "./window_ctx.hpp"

namespace jsar::example
{
  class Content;

  /**
   * Bar component that appears beneath each content for dragging.
   * Provides visual feedback and handles mouse interaction for spatial movement.
   */
  class BarComponent
  {
  public:
    BarComponent(WindowContext *windowCtx, Content *parentContent);
    ~BarComponent();

    /**
     * Render the bar component.
     */
    void render();

    /**
     * Update the bar position based on content position.
     */
    void updatePosition(const glm::vec3 &contentCenter);

    /**
     * Check if a screen point is within the bar bounds.
     */
    bool isPointInBounds(const glm::vec2 &screenPosition) const;

    /**
     * Set the hover state of the bar.
     */
    void setHovered(bool hovered)
    {
      isHovered_ = hovered;
    }

    /**
     * Set the dragging state of the bar.
     */
    void setDragging(bool dragging)
    {
      isDragging_ = dragging;
    }

  private:
    void initGLProgram();
    void resetCanvas();
    void uploadCanvas();
    void drawBar();

  private:
    WindowContext *windowCtx_;
    Content *parentContent_;

    glm::vec2 screenPosition_;
    bool isHovered_;
    bool isDragging_;

    // OpenGL resources
    GLuint vbo_;
    GLuint vao_;
    GLuint program_;
    GLuint texture_;

    // Skia resources
    sk_sp<SkSurface> surface_;
    SkCanvas *canvas_;
    SkPaint backgroundPaint_;
    SkPaint textPaint_;
    font::FontCacheManager fontMgr_;
    SkImageInfo imageInfo_;
    std::vector<uint8_t> pixels_;

    // Layout properties
    static constexpr int BAR_WIDTH = 200;
    static constexpr int BAR_HEIGHT = 30;
    static constexpr int BAR_OFFSET_Y = 10; // Offset below content

    // Vertices for the bar quad
    float vertices_[16];

    // Shaders
    const char *barVertSource_;
    const char *barFragSource_;
  };
}