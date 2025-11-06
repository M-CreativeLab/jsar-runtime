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
      class Gamepad;
      using GamepadBase = scripting_base::ObjectWrap<Gamepad, client_xr::Gamepad>;

      /**
       * Gamepad wrapper for V8 objects using scripting_base::ObjectWrap.
       *
       * This class wraps client_xr::Gamepad objects for use in V8 JavaScript execution contexts.
       * It provides the standard Gamepad API interface for WebXR input sources.
       */
      class Gamepad : public GamepadBase
      {
        using GamepadBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "Gamepad";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      public:
        Gamepad(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

      private:
        // Property getters
        void IdGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void IndexGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void ConnectedGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void TimestampGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void MappingGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void AxesGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void ButtonsGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      };
    }
  }
} // namespace endor
