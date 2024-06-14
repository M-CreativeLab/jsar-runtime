#include "binding.hpp"
#include "animation_frame_listener.hpp"

namespace bindings
{
  namespace renderer
  {
    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      AnimationFrameListener::Init(env, exports);
      return exports;
    }
  }
}
