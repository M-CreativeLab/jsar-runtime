#include "./xr_input_source.hpp"
#include "./xr_space.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    // XRInputSource implementation

    void XRInputSource::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, instance, "gamepad", &XRInputSource::GamepadGetter);
      InstanceReadonlyAccessor(isolate, instance, "gripSpace", &XRInputSource::GripSpaceGetter);
      InstanceReadonlyAccessor(isolate, instance, "hand", &XRInputSource::HandGetter);
      InstanceReadonlyAccessor(isolate, instance, "handedness", &XRInputSource::HandednessGetter);
      InstanceReadonlyAccessor(isolate, instance, "targetRayMode", &XRInputSource::TargetRayModeGetter);
      InstanceReadonlyAccessor(isolate, instance, "targetRaySpace", &XRInputSource::TargetRaySpaceGetter);
    }

    XRInputSource::XRInputSource(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : XRInputSourceBase(isolate, args, true)
    {
    }

    // Property getters

    void XRInputSource::GamepadGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRInputSource *inputSource = Unwrap(isolate, info.This());
      if (inputSource == nullptr || inputSource->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return actual Gamepad object from native input source
      cout << "inputSource.gamepad getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    void XRInputSource::GripSpaceGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRInputSource *inputSource = Unwrap(isolate, info.This());
      if (inputSource == nullptr || inputSource->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return actual XRSpace for grip from native input source
      cout << "inputSource.gripSpace getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    void XRInputSource::HandGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRInputSource *inputSource = Unwrap(isolate, info.This());
      if (inputSource == nullptr || inputSource->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return actual XRHand object from native input source
      cout << "inputSource.hand getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    void XRInputSource::HandednessGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRInputSource *inputSource = Unwrap(isolate, info.This());
      if (inputSource == nullptr || inputSource->inner() == nullptr)
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "none").ToLocalChecked());
        return;
      }

      // TODO: Get actual handedness from native input source (left, right, none)
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, "right").ToLocalChecked());
    }

    void XRInputSource::TargetRayModeGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRInputSource *inputSource = Unwrap(isolate, info.This());
      if (inputSource == nullptr || inputSource->inner() == nullptr)
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "gaze").ToLocalChecked());
        return;
      }

      // TODO: Get actual target ray mode from native input source (gaze, tracked-pointer, screen)
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, "tracked-pointer").ToLocalChecked());
    }

    void XRInputSource::TargetRaySpaceGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRInputSource *inputSource = Unwrap(isolate, info.This());
      if (inputSource == nullptr || inputSource->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return actual XRSpace for target ray from native input source
      cout << "inputSource.targetRaySpace getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    // XRInputSourceArray implementation

    Local<Array> XRInputSourceArray::NewInstance(Isolate *isolate,
                                                 const vector<shared_ptr<client_xr::XRInputSource>> &inputSources)
    {
      EscapableHandleScope scope(isolate);
      Local<Array> array = Array::New(isolate, inputSources.size());
      for (size_t i = 0; i < inputSources.size(); ++i)
      {
        Local<Object> inputSourceObj = XRInputSource::GetOrNewInstance(isolate, inputSources[i]);
        array->Set(isolate->GetCurrentContext(), i, inputSourceObj).Check();
      }
      return scope.Escape(array);
    }
  }
}