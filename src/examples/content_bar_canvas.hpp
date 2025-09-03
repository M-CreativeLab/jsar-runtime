#pragma once

#include <memory>
#include <vector>
#include <OpenGL/gl3.h>

#include <include/core/SkCanvas.h>
#include <include/core/SkSurface.h>
#include <include/core/SkImageInfo.h>

namespace jsar::example
{
  class Content;

  /**
   * ContentBarCanvas handles the UI rendering aspect of content bars using Skia Canvas.
   * Responsible for generating textures with Apple-style design that will be used by ContentBar3d.
   * All visual elements and UI logic are implemented here, decoupled from 3D scene integration.
   */
  class ContentBarCanvas
  {
  public:
    ContentBarCanvas();
    ~ContentBarCanvas();

    /**
     * Generate a texture for the bar with Apple-style design.
     * Returns the OpenGL texture ID that can be used by the 3D component.
     */
    GLuint generateBarTexture();

    /**
     * Update the visual state for a specific content (hover, dragging states).
     * This allows the canvas to render different visual feedback.
     */
    void updateContentState(Content *content, bool isHovered, bool isDragging);

    /**
     * Get the texture dimensions for the 3D component to use.
     */
    void getTextureDimensions(int &width, int &height) const;

    /**
     * Handle UI events that occur on the canvas.
     * This is part of the event proxy mechanism.
     */
    bool handleCanvasEvent(Content *content, const std::string &eventType, void *eventData);

  private:
    void createAppleStyleTexture();
    void renderBarDesign(SkCanvas *canvas, int width, int height);

  private:
    GLuint barTexture_;

    // Texture properties
    static constexpr int TEXTURE_WIDTH = 256;
    static constexpr int TEXTURE_HEIGHT = TEXTURE_WIDTH / 4; // Maintain aspect ratio for bar

    // Visual state tracking
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