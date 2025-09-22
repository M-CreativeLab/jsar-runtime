#include "./binding.hpp"
#include "./xr_space.hpp"
#include "./xr_session.hpp"
#include "./xr_frame.hpp"
#include "./xr_rigid_transform.hpp"
#include "./xr_pose.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr
  {
    void Initialize(Isolate *isolate, Local<Context> context)
    {
      HandleScope scope(isolate);
      Local<Object> global = context->Global();

      cout << "Initializing V8 WebXR bindings..." << endl;

      // Initialize WebXR classes
      auto xrSpaceConstructor = XRSpace::Initialize(isolate);
      auto xrReferenceSpaceConstructor = XRReferenceSpace::Initialize(isolate);
      auto xrSessionConstructor = XRSession::Initialize(isolate);
      auto xrFrameConstructor = XRFrame::Initialize(isolate);
      auto xrRigidTransformConstructor = XRRigidTransform::Initialize(isolate);
      auto xrPoseConstructor = XRPose::Initialize(isolate);
      auto xrViewerPoseConstructor = XRViewerPose::Initialize(isolate);

      // Set up global constructors
      global->Set(context, String::NewFromUtf8(isolate, "XRSpace").ToLocalChecked(), xrSpaceConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRReferenceSpace").ToLocalChecked(), xrReferenceSpaceConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRSession").ToLocalChecked(), xrSessionConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRFrame").ToLocalChecked(), xrFrameConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRRigidTransform").ToLocalChecked(), xrRigidTransformConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRPose").ToLocalChecked(), xrPoseConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRViewerPose").ToLocalChecked(), xrViewerPoseConstructor).Check();

      cout << "V8 WebXR bindings initialized successfully" << endl;
    }
  }
}