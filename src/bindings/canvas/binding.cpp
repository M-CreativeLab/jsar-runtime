#include "binding.hpp"
#include "image_data.hpp"
#include "image_bitmap.hpp"
#include "canvas.hpp"
#include "rendering_context2d.hpp"
#include "html_rendering_context.hpp"

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
      CanvasRenderingContext2D::Init(env, exports);
      HTMLRenderingContext::Init(env, exports);
      return exports;
    }
  } // namespace canvas
} // namespace bindings
