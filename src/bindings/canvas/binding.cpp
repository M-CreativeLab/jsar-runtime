#include "binding.hpp"
#include "image_data.hpp"
#include "image_bitmap.hpp"
#include "canvas.hpp"
#include "rendering_context2d.hpp"
#include "html_rendering_context.hpp"
#include "path2d.hpp"

namespace bindings
{
  namespace canvas
  {
    using namespace canvasbinding;

    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      ImageData::Init(env, exports);
      ImageBitmap::Init(env, exports);
      OffscreenCanvas::Init(env, exports);
      {
        // Initialize the rendering context classes for 2d
        CanvasRenderingContext2D::Init(env);
        OffscreenCanvasRenderingContext2D::Init(env);
      }
      HTMLRenderingContext::Init(env, exports);
      Path2D::Init(env, exports);
      return exports;
    }
  } // namespace canvas
} // namespace bindings
