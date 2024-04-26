#include "binding.hpp"
#include "device_native.hpp"
#include "frame.hpp"
#include "layer.hpp"
#include "session.hpp"
#include "pose.hpp"
#include "rigid_transform.hpp"
#include "space.hpp"

using namespace bindings;

Napi::Object bindings::InitWebxrModule(Napi::Env env, Napi::Object exports)
{
  // Device
  XRDeviceNative::Init(env, exports);

  // Frame
  XRFrame::Init(env, exports);

  // Layers
  XRLayer::Init(env, exports);
  XRWebGLLayer::Init(env, exports);

  // Poses
  XRPose::Init(env, exports);
  XRViewerPose::Init(env, exports);

  // Session
  XRSession::Init(env, exports);

  // RigidTransform
  XRRigidTransform::Init(env, exports);

  // Spaces
  XRSpace::Init(env, exports);
  XRReferenceSpace::Init(env, exports);
  XRViewSpace::Init(env, exports);

  // View
  XRView::Init(env, exports);
  return exports;
}
