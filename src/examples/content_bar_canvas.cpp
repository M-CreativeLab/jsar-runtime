#include <iostream>
#include <algorithm>

#include <include/core/SkPaint.h>
#include <include/core/SkRRect.h>
#include <include/core/SkPixmap.h>

#include "./content_bar_canvas.hpp"
#include "./content.hpp"

namespace jsar::example
{
  using namespace std;

  ContentBarCanvas::ContentBarCanvas()
      : barTexture_(0)
  {
    generateBarTexture();
  }

  ContentBarCanvas::~ContentBarCanvas()
  {
    if (barTexture_ != 0)
    {
      glDeleteTextures(1, &barTexture_);
    }
  }

  GLuint ContentBarCanvas::generateBarTexture()
  {
    if (barTexture_ != 0)
    {
      glDeleteTextures(1, &barTexture_);
    }

    createAppleStyleTexture();
    return barTexture_;
  }

  void ContentBarCanvas::updateContentState(Content *content, bool isHovered, bool isDragging)
  {
    auto it = find_if(contentStates_.begin(), contentStates_.end(), [content](const ContentState &state)
                      { return state.content == content; });

    if (it != contentStates_.end())
    {
      it->isHovered = isHovered;
      it->isDragging = isDragging;
    }
    else
    {
      ContentState newState(content);
      newState.isHovered = isHovered;
      newState.isDragging = isDragging;
      contentStates_.push_back(newState);
    }

    // Regenerate texture with updated state
    // In a more sophisticated implementation, this could be optimized
    // to only update when visual changes are needed
    generateBarTexture();
  }

  void ContentBarCanvas::getTextureDimensions(int &width, int &height) const
  {
    width = TEXTURE_WIDTH;
    height = TEXTURE_HEIGHT;
  }

  bool ContentBarCanvas::handleCanvasEvent(Content *content, const std::string &eventType, void *eventData)
  {
    // Simple event proxy implementation
    // In a more complex system, this could dispatch to specific UI components

    if (eventType == "hover")
    {
      bool *hovered = static_cast<bool *>(eventData);
      updateContentState(content, *hovered, false);
      return true;
    }
    else if (eventType == "drag")
    {
      bool *dragging = static_cast<bool *>(eventData);
      updateContentState(content, false, *dragging);
      return true;
    }

    return false; // Event not handled
  }

  void ContentBarCanvas::createAppleStyleTexture()
  {
    // Create Skia surface for rendering the bar texture with Apple design
    SkImageInfo info = SkImageInfo::MakeN32Premul(TEXTURE_WIDTH * 3,
                                                  TEXTURE_HEIGHT * 3);

    // Allocate pixel buffer for the texture
    size_t pixelBufferSize = info.computeMinByteSize();
    vector<uint8_t> pixelBuffer(pixelBufferSize);

    sk_sp<SkSurface> surface = SkSurfaces::WrapPixels(info, pixelBuffer.data(), info.minRowBytes());
    if (!surface)
    {
      cout << "Failed to create Skia surface for bar texture" << endl;
      return;
    }

    SkCanvas *canvas = surface->getCanvas();
    renderBarDesign(canvas, info.width(), info.height());

    // Get pixel data from Skia surface
    SkPixmap pixmap;
    if (!surface->peekPixels(&pixmap))
    {
      cout << "Failed to get pixels from Skia surface" << endl;
      return;
    }

    // Create OpenGL texture
    glGenTextures(1, &barTexture_);
    glBindTexture(GL_TEXTURE_2D, barTexture_);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 info.width(),
                 info.height(),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 pixmap.addr());

    // Set texture parameters for smooth scaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void ContentBarCanvas::renderBarDesign(SkCanvas *canvas, int width, int height)
  {
    canvas->clear(SK_ColorTRANSPARENT);

    // Apple-style bar design with rounded corners and opacity
    float cornerRadius = 12.0f;
    SkPaint paint;

    // Base bar with Apple's translucent white
    paint.setColor(SkColorSetARGB(0.8f * 255, 255, 255, 255)); // 80% opacity white
    paint.setAntiAlias(true);

    // Create rounded rectangle for the bar
    SkRect rect = SkRect::MakeWH(width, height);
    SkRRect roundedRect = SkRRect::MakeRectXY(rect, cornerRadius, cornerRadius);
    canvas->drawRRect(roundedRect, paint);

    // Add subtle inner shadow effect
    paint.setColor(SkColorSetARGB(0.5f * 255, 255, 255, 255)); // 50% opacity white
    SkRect innerRect = rect;
    innerRect.inset(1, 1);
    SkRRect innerRoundedRect = SkRRect::MakeRectXY(innerRect, cornerRadius - 1, cornerRadius - 1);
    canvas->drawRRect(innerRoundedRect, paint);
  }
}