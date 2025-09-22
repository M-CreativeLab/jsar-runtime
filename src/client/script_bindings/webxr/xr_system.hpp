#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_system.hpp>

namespace script_bindings
{
  namespace webxr
  {
    /**
     * XRSystem wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRSystem objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRSystem interface for session support checks
     * and session requests.
     */
    class XRSystem : public scripting_base::ObjectWrap<XRSystem, client_xr::XRSystem>
    {
    public:
      /**
       * The name of the XRSystem class for V8.
       */
      static std::string Name()
      {
        return "XRSystem";
      }

      /**
       * Configure the V8 function template with XRSystem properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 XRSystem instance from a native client_xr::XRSystem.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRSystem> nativeSystem);

      /**
       * Initialize the XRSystem class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      XRSystem(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Methods
      static void IsSessionSupported(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void RequestSession(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}