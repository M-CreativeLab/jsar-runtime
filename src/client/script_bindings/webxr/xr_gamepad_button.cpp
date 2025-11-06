#include "./xr_gamepad_button.hpp"

using namespace std;
using namespace v8;

namespace endor
{
  namespace script_bindings
  {
    namespace webxr_bindings
    {
      void GamepadButton::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
      {
        HandleScope scope(isolate);
        Local<ObjectTemplate> instance = tpl->InstanceTemplate();

        // Add property accessors
        InstanceReadonlyAccessor(isolate, instance, "pressed", &GamepadButton::PressedGetter);
        InstanceReadonlyAccessor(isolate, instance, "touched", &GamepadButton::TouchedGetter);
        InstanceReadonlyAccessor(isolate, instance, "value", &GamepadButton::ValueGetter);
      }

      GamepadButton::GamepadButton(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
          : GamepadButtonBase(isolate, args)
      {
      }

      void GamepadButton::PressedGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(Boolean::New(isolate, handle()->pressed()));
      }

      void GamepadButton::TouchedGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(Boolean::New(isolate, handle()->touched()));
      }

      void GamepadButton::ValueGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(Number::New(isolate, handle()->value()));
      }
    }
  }
} // namespace endor
