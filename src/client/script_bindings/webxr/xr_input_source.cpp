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

    // static
    void XRInputSource::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add property accessors
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "gamepad").ToLocalChecked(),
                                    GamepadGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "gripSpace").ToLocalChecked(),
                                    GripSpaceGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "hand").ToLocalChecked(),
                                    HandGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "handedness").ToLocalChecked(),
                                    HandednessGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "targetRayMode").ToLocalChecked(),
                                    TargetRayModeGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "targetRaySpace").ToLocalChecked(),
                                    TargetRaySpaceGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);
    }

    // static
    Local<Object> XRInputSource::NewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRInputSource> nativeInputSource)
    {
      EscapableHandleScope scope(isolate);

      if (nativeInputSource == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<XRInputSource, client_xr::XRInputSource>::NewInstance(isolate, nativeInputSource).As<Object>());
    }

    // static
    Local<Object> XRInputSource::GetOrNewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRInputSource> nativeInputSource)
    {
      // For now, just create a new instance. In a full implementation,
      // we might cache instances to maintain object identity.
      return NewInstance(isolate, nativeInputSource);
    }

    // static
    Local<Function> XRInputSource::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<XRInputSource, client_xr::XRInputSource>::Initialize(isolate);
    }

    XRInputSource::XRInputSource(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<XRInputSource, client_xr::XRInputSource>(isolate, args)
    {
    }

    // Property getters

    // static
    void XRInputSource::GamepadGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRInputSource *inputSource = scripting_base::ObjectWrap<XRInputSource, client_xr::XRInputSource>::Unwrap(info.This());
      if (inputSource == nullptr || inputSource->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return actual Gamepad object from native input source
      cout << "inputSource.gamepad getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    // static
    void XRInputSource::GripSpaceGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRInputSource *inputSource = scripting_base::ObjectWrap<XRInputSource, client_xr::XRInputSource>::Unwrap(info.This());
      if (inputSource == nullptr || inputSource->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return actual XRSpace for grip from native input source
      cout << "inputSource.gripSpace getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    // static
    void XRInputSource::HandGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRInputSource *inputSource = scripting_base::ObjectWrap<XRInputSource, client_xr::XRInputSource>::Unwrap(info.This());
      if (inputSource == nullptr || inputSource->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return actual XRHand object from native input source
      cout << "inputSource.hand getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    // static
    void XRInputSource::HandednessGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRInputSource *inputSource = scripting_base::ObjectWrap<XRInputSource, client_xr::XRInputSource>::Unwrap(info.This());
      if (inputSource == nullptr || inputSource->inner() == nullptr)
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "none").ToLocalChecked());
        return;
      }

      // TODO: Get actual handedness from native input source (left, right, none)
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, "right").ToLocalChecked());
    }

    // static
    void XRInputSource::TargetRayModeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRInputSource *inputSource = scripting_base::ObjectWrap<XRInputSource, client_xr::XRInputSource>::Unwrap(info.This());
      if (inputSource == nullptr || inputSource->inner() == nullptr)
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "gaze").ToLocalChecked());
        return;
      }

      // TODO: Get actual target ray mode from native input source (gaze, tracked-pointer, screen)
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, "tracked-pointer").ToLocalChecked());
    }

    // static
    void XRInputSource::TargetRaySpaceGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRInputSource *inputSource = scripting_base::ObjectWrap<XRInputSource, client_xr::XRInputSource>::Unwrap(info.This());
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

    // static
    Local<Array> XRInputSourceArray::NewInstance(Isolate *isolate, const std::vector<std::shared_ptr<client_xr::XRInputSource>> &inputSources)
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