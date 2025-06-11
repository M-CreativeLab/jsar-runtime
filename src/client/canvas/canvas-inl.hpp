#include <memory>
#include <skia/include/core/SkCanvas.h>

#include "./canvas.hpp"
#include "./rendering_context2d.hpp"

namespace canvas
{
  template <typename T>
  std::shared_ptr<RenderingContextBase<T>> CanvasBase<T>::getContext(RenderingContextType type)
  {
    if (renderingContext2d == nullptr)
    {
      resetSkSurface(); // Reset the SkSurface when a request to get new rendering context.
      switch (type)
      {
      case RenderingContextType::RenderingContext2D:
        renderingContext2d = std::make_shared<CanvasRenderingContext2D<T>>(getPtr());
        break;
      default:
        break;
      }
    }
    return renderingContext2d;
  }

  template <typename T>
  void CanvasBase<T>::resetSkSurface()
  {
    auto imageInfo = SkImageInfo::MakeN32Premul(widthToSet, heightToSet);
    if (skSurface != nullptr)
      skSurface.reset();

    bitmap_->allocPixels(imageInfo);
    skSurface = SkSurfaces::WrapPixels(bitmap_->info(),
                                       bitmap_->getPixels(),
                                       bitmap_->rowBytes());
    assert(skSurface != nullptr && "Failed to create Skia surface for the canvas.");

    // Reset the rendering context if created.
    if (renderingContext2d != nullptr)
      renderingContext2d->reset(skSurface);
  }
}
