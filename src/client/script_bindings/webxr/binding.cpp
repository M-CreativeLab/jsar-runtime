#include "./binding.hpp"
#include "./xr_space.hpp"
#include "./xr_session.hpp"
#include "./xr_frame.hpp"
#include "./xr_rigid_transform.hpp"
#include "./xr_pose.hpp"
#include "./xr_viewport.hpp"
#include "./xr_view.hpp"
#include "./xr_system.hpp"
#include "./xr_render_state.hpp"
#include "./xr_layer.hpp"
#include "./xr_input_source.hpp"
#include "./xr_hand.hpp"
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
      auto xrViewportConstructor = XRViewport::Initialize(isolate);
      auto xrViewConstructor = XRView::Initialize(isolate);
      auto xrSystemConstructor = XRSystem::Initialize(isolate);
      auto xrRenderStateConstructor = XRRenderState::Initialize(isolate);
      auto xrLayerConstructor = XRLayer::Initialize(isolate);
      auto xrWebGLLayerConstructor = XRWebGLLayer::Initialize(isolate);
      auto xrInputSourceConstructor = XRInputSource::Initialize(isolate);
      auto xrHandConstructor = XRHand::Initialize(isolate);

      // Set up global constructors
      global->Set(context, String::NewFromUtf8(isolate, "XRSpace").ToLocalChecked(), xrSpaceConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRReferenceSpace").ToLocalChecked(), xrReferenceSpaceConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRSession").ToLocalChecked(), xrSessionConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRFrame").ToLocalChecked(), xrFrameConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRRigidTransform").ToLocalChecked(), xrRigidTransformConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRPose").ToLocalChecked(), xrPoseConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRViewerPose").ToLocalChecked(), xrViewerPoseConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRViewport").ToLocalChecked(), xrViewportConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRView").ToLocalChecked(), xrViewConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRSystem").ToLocalChecked(), xrSystemConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRRenderState").ToLocalChecked(), xrRenderStateConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRLayer").ToLocalChecked(), xrLayerConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRWebGLLayer").ToLocalChecked(), xrWebGLLayerConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRInputSource").ToLocalChecked(), xrInputSourceConstructor).Check();
      global->Set(context, String::NewFromUtf8(isolate, "XRHand").ToLocalChecked(), xrHandConstructor).Check();

      cout << "V8 WebXR bindings initialized successfully" << endl;
    }
  }
}