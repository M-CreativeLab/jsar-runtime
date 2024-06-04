#include "binding.hpp"
#include "render_loop.hpp"
#include "animation_frame_listener.hpp"

namespace bindings
{
  namespace renderer
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      RenderLoop::Init(env, exports);
      AnimationFrameListener::Init(env, exports);
      return exports;
    }
  }
}
