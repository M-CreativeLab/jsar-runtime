#include <iostream>
#include <algorithm>

#include <include/core/SkPaint.h>
#include <include/core/SkRRect.h>
#include <include/core/SkPixmap.h>

#include "./canvas_system.hpp"
#include "./content.hpp"

namespace jsar::example
{
  using namespace std;

  CanvasSystem::CanvasSystem(int width, int height)
      : width_(width)
      , height_(height)
      , texture_(0)
  {
    initializeSkiaSurface();
    createOpenGLTexture();
    updateTexture();
  }

  CanvasSystem::~CanvasSystem()
  {
    if (texture_ != 0)
    {
      glDeleteTextures(1, &texture_);
    }
  }

  void CanvasSystem::registerRayEventHandler(RayEventHandler handler)
  {
    rayEventHandlers_.push_back(handler);
  }

  bool CanvasSystem::processRayEvent(const RayEvent &event)
  {
    for (auto &handler : rayEventHandlers_)
    {
      if (handler(event))
      {
        return true; // Event handled
      }
    }
    return false;
  }

  void CanvasSystem::getTextureDimensions(int &width, int &height) const
  {
    width = width_;
    height = height_;
  }

  void CanvasSystem::updateContentState(Content *content, bool isHovered, bool isDragging)
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

    // Update texture with new state
    updateTexture();
  }

  void CanvasSystem::updateTexture()
  {
    if (!surface_)
    {
      return;
    }

    SkCanvas *canvas = surface_->getCanvas();
    drawCanvas(canvas);

    // Get pixel data from Skia surface
    SkPixmap pixmap;
    if (!surface_->peekPixels(&pixmap))
    {
      cout << "Failed to get pixels from Skia surface" << endl;
      return;
    }

    // Update OpenGL texture
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, pixmap.addr());
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void CanvasSystem::render()
  {
    // This method is called by ScreenRenderer for screen-space rendering
    // For now, we just ensure the texture is up to date
    updateTexture();
  }

  void CanvasSystem::handleMouseButton(GLFWwindow *window, int button, int action, double xpos, double ypos)
  {
    // Convert screen coordinates to ray event for consistency
    // This is a simplified conversion - in a real implementation,
    // you'd need proper coordinate transformation
    RayEvent event;
    event.type = (action == GLFW_PRESS) ? RayEventType::Down : RayEventType::Up;
    event.rayOrigin = glm::vec3(0, 0, 0);
    event.rayDirection = glm::vec3(0, 0, -1);
    event.localPosition = glm::vec2(xpos / width_, ypos / height_);

    processRayEvent(event);
  }

  void CanvasSystem::drawCanvas(SkCanvas *canvas)
  {
    // Default implementation draws Apple-style bars
    renderAppleStyleBar(canvas);
  }

  void CanvasSystem::initializeSkiaSurface()
  {
    SkImageInfo info = SkImageInfo::MakeN32Premul(width_, height_);

    size_t pixelBufferSize = info.computeMinByteSize();
    pixelBuffer_.resize(pixelBufferSize);

    surface_ = SkSurfaces::WrapPixels(info, pixelBuffer_.data(), info.minRowBytes());
    if (!surface_)
    {
      cout << "Failed to create Skia surface for canvas" << endl;
    }
  }

  void CanvasSystem::createOpenGLTexture()
  {
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);

    // Initialize with transparent pixels
    vector<uint8_t> transparentPixels(width_ * height_ * 4, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, transparentPixels.data());

    // Set texture parameters for smooth scaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void CanvasSystem::renderAppleStyleBar(SkCanvas *canvas)
  {
    canvas->clear(SK_ColorTRANSPARENT);

    // Apple-style bar design with rounded corners and opacity
    float cornerRadius = 12.0f;
    SkPaint paint;

    // Determine visual state based on content states
    bool anyHovered = any_of(contentStates_.begin(), contentStates_.end(), [](const ContentState &state)
                             { return state.isHovered; });
    bool anyDragging = any_of(contentStates_.begin(), contentStates_.end(), [](const ContentState &state)
                              { return state.isDragging; });

    // Base bar color changes based on state
    SkColor baseColor;
    if (anyDragging)
    {
      baseColor = SkColorSetARGB(0.9f * 255, 100, 150, 255); // Blue when dragging
    }
    else if (anyHovered)
    {
      baseColor = SkColorSetARGB(0.85f * 255, 255, 255, 255); // Brighter when hovered
    }
    else
    {
      baseColor = SkColorSetARGB(0.8f * 255, 255, 255, 255); // Default translucent white
    }

    paint.setColor(baseColor);
    paint.setAntiAlias(true);

    // Create rounded rectangle for the bar
    SkRect rect = SkRect::MakeWH(width_, height_);
    SkRRect roundedRect = SkRRect::MakeRectXY(rect, cornerRadius, cornerRadius);
    canvas->drawRRect(roundedRect, paint);

    // Add subtle inner shadow effect
    paint.setColor(SkColorSetARGB(0.5f * 255, 255, 255, 255));
    SkRect innerRect = rect;
    innerRect.inset(1, 1);
    SkRRect innerRoundedRect = SkRRect::MakeRectXY(innerRect, cornerRadius - 1, cornerRadius - 1);
    canvas->drawRRect(innerRoundedRect, paint);
  }
}