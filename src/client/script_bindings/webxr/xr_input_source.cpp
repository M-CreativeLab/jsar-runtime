#include <iostream>
#include "./xr_input_source.hpp"
#include "./xr_space.hpp"
#include "./xr_hand.hpp"

using namespace std;
using namespace v8;

namespace endor
{
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
          : XRInputSourceBase(isolate, args)
      {
      }

      // Property getters

      void XRInputSource::GamepadGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        cerr << "Warning: XRInputSource.gamepad is not implemented yet." << endl;
        info.GetReturnValue().SetNull();
      }

      void XRInputSource::GripSpaceGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        auto gripSpace = handle()->gripSpace();
        if (gripSpace == nullptr)
        {
          info.GetReturnValue().SetNull();
        }
        else
        {
          info.GetReturnValue().Set(XRSpace::GetOrNewInstance(isolate, gripSpace));
        }
      }

      void XRInputSource::HandGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(XRHand::NewInstance(isolate,
                                                      handle()->hand()));
      }

      void XRInputSource::HandednessGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        auto handedness = handle()->handedness();
        switch (handedness)
        {
        case client_xr::XRHandedness::Left:
          info.GetReturnValue().Set(String::NewFromUtf8(isolate, "left").ToLocalChecked());
          break;
        case client_xr::XRHandedness::Right:
          info.GetReturnValue().Set(String::NewFromUtf8(isolate, "right").ToLocalChecked());
          break;
        case client_xr::XRHandedness::None:
        default:
          info.GetReturnValue().Set(String::NewFromUtf8(isolate, "none").ToLocalChecked());
          break;
        }
      }

      void XRInputSource::TargetRayModeGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        auto mode = handle()->targetRayMode();
        switch (mode)
        {
        case client_xr::XRTargetRayMode::Gaze:
          info.GetReturnValue().Set(String::NewFromUtf8(isolate, "gaze").ToLocalChecked());
          break;
        case client_xr::XRTargetRayMode::TrackedPointer:
          info.GetReturnValue().Set(String::NewFromUtf8(isolate, "tracked-pointer").ToLocalChecked());
          break;
        case client_xr::XRTargetRayMode::Screen:
          info.GetReturnValue().Set(String::NewFromUtf8(isolate, "screen").ToLocalChecked());
          break;
        default:
          info.GetReturnValue().Set(String::NewFromUtf8(isolate, "unknown").ToLocalChecked());
          break;
        }
      }

      void XRInputSource::TargetRaySpaceGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        auto space = handle()->targetRaySpace();
        info.GetReturnValue().Set(XRSpace::GetOrNewInstance(isolate, space));
      }

      // XRInputSourceArray implementation

      Local<Array> XRInputSourceArray::NewInstance(Isolate *isolate,
                                                   const client_xr::XRInputSourceArray &inputSources)
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
} // namespace endor
