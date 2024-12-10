#include "./binding.hpp"
#include "./system.hpp"
#include "./frame.hpp"
#include "./hand.hpp"
#include "./input_source.hpp"
#include "./layer.hpp"
#include "./session.hpp"
#include "./pose.hpp"
#include "./rigid_transform.hpp"
#include "./render_state.hpp"
#include "./space.hpp"
#include "./view.hpp"
#include "./viewport.hpp"

using namespace bindings;

namespace bindings
{
  namespace webxr
  {
    Napi::Value GetXRSystem(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      return XRSystem::NewInstance(env);
    }

    Napi::Object InitModule(Napi::Env env, Napi::Object exports)
    {
      // System
      XRSystem::Init(env);

      // Frame
      XRFrame::Init(env, exports);

      // Input Source
      XRHand::Init(env, exports);
      XRInputSource::Init(env, exports);

      // Layers
      XRLayer::Init(env);
      XRWebGLLayer::Init(env);

      // Poses
      XRPose::Init(env, exports);
      XRViewerPose::Init(env, exports);

      // Session
      XRSession::Init(env, exports);
      XRRenderState::Init(env, exports);

      // RigidTransform
      XRRigidTransform::Init(env, exports);

      // Spaces
      XRSpace::Init(env, exports);
      XRReferenceSpace::Init(env, exports);
      XRViewSpace::Init(env, exports);
      XRJointSpace::Init(env, exports);
      XRTargetRayOrGripSpace::Init(env, exports);

      // View
      XRView::Init(env, exports);

      // Viewport
      XRViewport::Init(env, exports);

      // Set method
      exports.Set(Napi::String::New(env, "getXRSystem"), Napi::Function::New(env, GetXRSystem));
      return exports;
    }
  }
}
