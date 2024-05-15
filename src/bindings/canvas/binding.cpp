#include "binding.hpp"
#include "canvas.hpp"

Napi::Object InitCanvasModule(Napi::Env env, Napi::Object exports)
{
  canvas::OffscreenCanvas::Init(env, exports);
  return exports;
}
