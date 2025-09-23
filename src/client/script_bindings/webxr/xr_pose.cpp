#include "./xr_pose.hpp"
#include "./xr_rigid_transform.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    // XRPose implementation

    // static
    void XRPose::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add property accessors
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "transform").ToLocalChecked(),
                                    TransformGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "emulatedPosition").ToLocalChecked(),
                                    EmulatedPositionGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);
    }

    // static
    Local<Object> XRPose::NewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRPose> nativePose)
    {
      EscapableHandleScope scope(isolate);

      if (nativePose == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<XRPose, client_xr::XRPose>::NewInstance(isolate, nativePose).As<Object>());
    }

    // static
    Local<Function> XRPose::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<XRPose, client_xr::XRPose>::Initialize(isolate);
    }

    XRPose::XRPose(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<XRPose, client_xr::XRPose>(isolate, args)
    {
    }

    // Property getters

    // static
    void XRPose::TransformGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRPose *pose = scripting_base::ObjectWrap<XRPose, client_xr::XRPose>::Unwrap(info.This());
      if (pose == nullptr || pose->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return XRRigidTransform for the pose's transform
      cout << "pose.transform getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    // static
    void XRPose::EmulatedPositionGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRPose *pose = scripting_base::ObjectWrap<XRPose, client_xr::XRPose>::Unwrap(info.This());
      if (pose == nullptr || pose->inner() == nullptr)
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
        return;
      }

      bool emulatedPosition = pose->inner()->emulatedPosition;
      info.GetReturnValue().Set(Boolean::New(isolate, emulatedPosition));
    }

    // XRViewerPose implementation

    // static
    void XRViewerPose::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add property accessors
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "views").ToLocalChecked(),
                                    ViewsGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);
    }

    // static
    Local<Object> XRViewerPose::NewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRViewerPose> nativeViewerPose)
    {
      EscapableHandleScope scope(isolate);

      if (nativeViewerPose == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<XRViewerPose, client_xr::XRViewerPose, XRPose>::NewInstance(isolate, nativeViewerPose).As<Object>());
    }

    // static
    Local<Function> XRViewerPose::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<XRViewerPose, client_xr::XRViewerPose, XRPose>::Initialize(isolate);
    }

    XRViewerPose::XRViewerPose(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<XRViewerPose, client_xr::XRViewerPose, XRPose>(isolate, args)
    {
    }

    // Property getters

    // static
    void XRViewerPose::ViewsGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRViewerPose *viewerPose = scripting_base::ObjectWrap<XRViewerPose, client_xr::XRViewerPose, XRPose>::Unwrap(info.This());
      if (viewerPose == nullptr || viewerPose->inner() == nullptr)
      {
        info.GetReturnValue().Set(Array::New(isolate, 0));
        return;
      }

      // TODO: Return array of XRView objects
      cout << "viewerPose.views getter called" << endl;
      info.GetReturnValue().Set(Array::New(isolate, 0));
    }
  }
}