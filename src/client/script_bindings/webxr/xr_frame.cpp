#include <iostream>
#include "./xr_frame.hpp"
#include "./xr_space.hpp"
#include "./xr_session.hpp"
#include "./xr_pose.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    void XRFrame::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
      Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, instanceTemplate, "session", &XRFrame::SessionGetter);

      // Add methods
      InstanceMethod(isolate, instanceTemplate, "getPose", &XRFrame::GetPose);
      InstanceMethod(isolate, instanceTemplate, "getViewerPose", &XRFrame::GetViewerPose);
      InstanceMethod(isolate, instanceTemplate, "createAnchor", &XRFrame::CreateAnchor);
      InstanceMethod(isolate, instanceTemplate, "getHitTestResults", &XRFrame::GetHitTestResults);
      InstanceMethod(isolate, instanceTemplate, "getHitTestResultsForTransientInput", &XRFrame::GetHitTestResultsForTransientInput);
      InstanceMethod(isolate, instanceTemplate, "fillPoses", &XRFrame::FillPoses);
      InstanceMethod(isolate, instanceTemplate, "fillJointRadii", &XRFrame::FillJointRadii);
    }

    Local<Object> XRFrame::NewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRFrame> nativeFrame)
    {
      EscapableHandleScope scope(isolate);
      assert(nativeFrame != nullptr && "nativeFrame must not be null");
      return scope.Escape(XRFrameBase::NewInstance(isolate, nativeFrame).As<Object>());
    }

    // Property getters

    void XRFrame::SessionGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto session = handle()->session();
      if (session == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }
      else
      {
        auto jsSession = XRSession::GetOrNewInstance(isolate, session);
        info.GetReturnValue().Set(jsSession);
        return;
      }
    }

    // Methods

    void XRFrame::GetPose(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "getPose", "Requires 2 arguments")));
        return;
      }

      if (XRSpace::IsInstanceOf(isolate, info[0]) == false ||
          XRSpace::IsInstanceOf(isolate, info[1]) == false)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "getPose", "Arguments must be `XRSpace` objects")));
        return;
      }

      auto space = XRSpace::Unwrap(isolate, info[0].As<Object>());
      auto baseSpace = XRSpace::Unwrap(isolate, info[1].As<Object>());

      auto pose = handle()->getPose(space->handle(), baseSpace->handle());
      if (pose == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }
      else
      {
        auto jsPose = XRPose::GetOrNewInstance(isolate, pose);
        info.GetReturnValue().Set(jsPose);
        return;
      }
    }

    void XRFrame::GetViewerPose(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "getViewerPose requires 1 argument").ToLocalChecked()));
        return;
      }

      if (XRReferenceSpace::IsInstanceOf(isolate, info[0]) == false)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "getViewerPose", "Argument must be an `XRReferenceSpace` object")));
        return;
      }

      auto referenceSpace = XRReferenceSpace::Unwrap(isolate, info[0].As<Object>());
      auto viewerPose = handle()->getViewerPose(referenceSpace->handle());
      if (viewerPose == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }
      else
      {
        auto jsViewerPose = XRViewerPose::GetOrNewInstance(isolate, viewerPose);
        info.GetReturnValue().Set(jsViewerPose);
        return;
      }
    }

    void XRFrame::CreateAnchor(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "createAnchor", "Not implemented")));
    }

    void XRFrame::GetHitTestResults(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "getHitTestResults", "Not implemented")));
    }

    void XRFrame::GetHitTestResultsForTransientInput(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "getHitTestResultsForTransientInput", "Not implemented")));
    }

    void XRFrame::FillPoses(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "fillPoses", "Not implemented")));
    }

    void XRFrame::FillJointRadii(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "fillJointRadii", "Not implemented")));
    }
  }
}