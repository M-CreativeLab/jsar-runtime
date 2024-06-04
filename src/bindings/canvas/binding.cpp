#include "binding.hpp"
#include "canvas.hpp"
#include "rendering_context2d.hpp"

namespace bindings
{
  namespace canvas
  {
    using namespace canvasbinding;

    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      OffscreenCanvas::Init(env, exports);
      CanvasRenderingContext2D::Init(env, exports);
      return exports;
    }
  } // namespace canvas
} // namespace bindings
