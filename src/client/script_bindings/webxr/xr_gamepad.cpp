#include "./xr_gamepad.hpp"
#include "./xr_gamepad_button.hpp"

using namespace std;
using namespace v8;

namespace endor
{
  namespace script_bindings
  {
    namespace webxr_bindings
    {
      void Gamepad::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
      {
        HandleScope scope(isolate);
        Local<ObjectTemplate> instance = tpl->InstanceTemplate();

        // Add property accessors
        InstanceReadonlyAccessor(isolate, instance, "id", &Gamepad::IdGetter);
        InstanceReadonlyAccessor(isolate, instance, "index", &Gamepad::IndexGetter);
        InstanceReadonlyAccessor(isolate, instance, "connected", &Gamepad::ConnectedGetter);
        InstanceReadonlyAccessor(isolate, instance, "timestamp", &Gamepad::TimestampGetter);
        InstanceReadonlyAccessor(isolate, instance, "mapping", &Gamepad::MappingGetter);
        InstanceReadonlyAccessor(isolate, instance, "axes", &Gamepad::AxesGetter);
        InstanceReadonlyAccessor(isolate, instance, "buttons", &Gamepad::ButtonsGetter);
      }

      Gamepad::Gamepad(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
          : GamepadBase(isolate, args)
      {
      }

      void Gamepad::IdGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        const auto &id = handle()->id();
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, id.c_str()).ToLocalChecked());
      }

      void Gamepad::IndexGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(Number::New(isolate, handle()->index()));
      }

      void Gamepad::ConnectedGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(Boolean::New(isolate, handle()->connected()));
      }

      void Gamepad::TimestampGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        info.GetReturnValue().Set(Number::New(isolate, handle()->timestamp()));
      }

      void Gamepad::MappingGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        const auto &mapping = handle()->mapping();
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, mapping.c_str()).ToLocalChecked());
      }

      void Gamepad::AxesGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        const auto &axes = handle()->axes();
        Local<Array> axesArray = Array::New(isolate, axes.size());
        for (size_t i = 0; i < axes.size(); ++i)
        {
          axesArray->Set(context, i, Number::New(isolate, axes[i])).Check();
        }
        info.GetReturnValue().Set(axesArray);
      }

      void Gamepad::ButtonsGetter(const PropertyCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);
        Local<Context> context = isolate->GetCurrentContext();

        const auto &buttons = handle()->buttons();
        Local<Array> buttonsArray = Array::New(isolate, buttons.size());
        for (size_t i = 0; i < buttons.size(); ++i)
        {
          // Create a new GamepadButton object for each button
          auto buttonPtr = make_shared<client_xr::GamepadButton>(buttons[i]);
          Local<Object> buttonObj = GamepadButton::GetOrNewInstance(isolate, buttonPtr);
          buttonsArray->Set(context, i, buttonObj).Check();
        }
        info.GetReturnValue().Set(buttonsArray);
      }
    }
  }
} // namespace endor
