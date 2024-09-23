#include "./canvas.hpp"

using namespace std;

namespace canvas
{
  template <typename T>
  shared_ptr<RenderingContextBase<T>> CanvasBase<T>::getContext(RenderingContextType type)
  {
    if (renderingContext == nullptr)
    {
      resetSkSurface(); // Reset the SkSurface when a request to get new rendering context.
      switch (type)
      {
      case RenderingContextType::RenderingContext2D:
        renderingContext = make_shared<CanvasRenderingContext2D<T>>(getPtr());
        break;
      default:
        break;
      }
    }
    return renderingContext;
  }

  template <typename T>
  void CanvasBase<T>::resetSkSurface()
  {
    auto imageInfo = SkImageInfo::MakeN32Premul(widthToSet, heightToSet);
    if (skSurface != nullptr)
      skSurface.reset();
    skSurface = SkSurfaces::Raster(imageInfo);
  }
}
