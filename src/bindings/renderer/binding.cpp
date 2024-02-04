#include "binding.hpp"
#include "render_loop.hpp"

Napi::Object InitRendererModule(Napi::Env env, Napi::Object exports)
{
  renderer::RenderLoop::Init(env, exports);
  return exports;
}
