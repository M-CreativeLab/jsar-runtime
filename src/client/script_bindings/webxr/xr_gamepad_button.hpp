#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_gamepad.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace webxr_bindings
    {
      class GamepadButton;
      using GamepadButtonBase = scripting_base::ObjectWrap<GamepadButton, client_xr::GamepadButton>;

      /**
       * GamepadButton wrapper for V8 objects using scripting_base::ObjectWrap.
       *
       * This class wraps client_xr::GamepadButton objects for use in V8 JavaScript execution contexts.
       * It provides the standard Gamepad API GamepadButton interface.
       */
      class GamepadButton : public GamepadButtonBase
      {
        using GamepadButtonBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "GamepadButton";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      public:
        GamepadButton(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

      private:
        // Property getters
        void PressedGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void TouchedGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void ValueGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      };
    }
  }
} // namespace endor
