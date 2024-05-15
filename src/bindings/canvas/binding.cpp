#include "binding.hpp"
#include "canvas.hpp"
#include "rendering_context2d.hpp"

Napi::Object InitCanvasModule(Napi::Env env, Napi::Object exports)
{
  canvas::OffscreenCanvas::Init(env, exports);
  canvas::CanvasRenderingContext2D::Init(env, exports);
  return exports;
}
