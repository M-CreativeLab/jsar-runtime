#pragma once

#include <memory>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include "./rendering_context.hpp"
#include "./image_source.hpp"

namespace canvas
{
  template <typename T>
  class CanvasBase : public ImageSource,
                     public std::enable_shared_from_this<T>
  {
  public:
    friend class canvas::CanvasRenderingContext2D<T>;

  public:
    CanvasBase() : ImageSource(), widthToSet(1024), heightToSet(1024) {}

  public:
    /**
     * Create or get the rendering context with the specified type.
     *
     * @param type The type of the rendering context: 2d, webgl, webgl2.
     * @returns The rendering context.
     */
    std::shared_ptr<RenderingContextBase<T>> getContext(RenderingContextType type);

  public:
    size_t width() override
    {
      return skSurface == nullptr ? 0 : skSurface->width();
    }
    size_t height() override
    {
      return skSurface == nullptr ? 0 : skSurface->height();
    }
    bool readPixels(SkPixmap &dst) override
    {
      if (skSurface == nullptr)
        return false;
      else
        return skSurface->peekPixels(&dst);
    }

  public:
    void setWidth(uint32_t width)
    {
      widthToSet = width;
      resize();
    }
    void setHeight(uint32_t height)
    {
      heightToSet = height;
      resize();
    }
    /**
     * Resize the canvas.
     */
    void resize()
    {
      resetSkSurface();
    }

  protected:
    std::shared_ptr<T> getPtr() { return this->shared_from_this(); }
    void resetSkSurface();
    SkBitmap *toSkBitmap();

  protected:
    sk_sp<SkSurface> skSurface;
    std::shared_ptr<RenderingContextBase<T>> renderingContext;
    uint32_t widthToSet;
    uint32_t heightToSet;
  };

  class Canvas : public CanvasBase<Canvas>
  {
  public:
    using CanvasBase::CanvasBase;

  public:
    std::string toDataURL(std::string type, double encoderOptions);
  };

  class OffscreenCanvas : public CanvasBase<OffscreenCanvas>
  {
  public:
    using CanvasBase::CanvasBase;
    OffscreenCanvas(uint32_t width, uint32_t height);

  public:
    void commit();
  };
}

#include "./canvas-inl.hpp"
