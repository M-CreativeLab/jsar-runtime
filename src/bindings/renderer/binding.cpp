#include "binding.hpp"
#include "render_loop.hpp"
#include "animation_frame_listener.hpp"

Napi::Object InitRendererModule(Napi::Env env, Napi::Object exports)
{
  renderer::RenderLoop::Init(env, exports);
  bindings::AnimationFrameListener::Init(env, exports);
  return exports;
}
