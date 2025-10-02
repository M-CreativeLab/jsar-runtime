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

    void XRPose::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, instance, "transform", &XRPose::TransformGetter);
      InstanceReadonlyAccessor(isolate, instance, "emulatedPosition", &XRPose::EmulatedPositionGetter);
    }

    // Property getters

    void XRPose::TransformGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRPose *pose = Unwrap(isolate, info.This());
      if (pose == nullptr || pose->handle() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return XRRigidTransform for the pose's transform
      cout << "pose.transform getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    void XRPose::EmulatedPositionGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRPose *pose = Unwrap(isolate, info.This());
      if (pose == nullptr || pose->handle() == nullptr)
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
        return;
      }

      bool emulatedPosition = pose->handle()->emulatedPosition;
      info.GetReturnValue().Set(Boolean::New(isolate, emulatedPosition));
    }

    // XRViewerPose implementation

    void XRViewerPose::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, instance, "views", &XRViewerPose::ViewsGetter);
    }

    // Property getters

    void XRViewerPose::ViewsGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      // TODO: Return array of XRView objects
      cout << "viewerPose.views getter called" << endl;
      info.GetReturnValue().Set(Array::New(isolate, 0));
    }
  }
}
