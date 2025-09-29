#pragma once

#include <memory>
#include <optional>
#include <common/xr/common.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_system.hpp>

namespace script_bindings
{
  namespace webxr_bindings
  {
    class XRSystem;
    using XRSystemBase = scripting_base::ObjectWrap<XRSystem, client_xr::XRSystem>;

    /**
     * XRSystem wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRSystem objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRSystem interface for session support checks
     * and session requests.
     */
    class XRSystem : public XRSystemBase
    {
    public:
      /**
       * The name of the XRSystem class for V8.
       */
      static std::string Name()
      {
        return "XRSystem";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<client_xr::XRSystem> handle);

    public:
      XRSystem(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Methods
      void IsSessionSupported(const v8::FunctionCallbackInfo<v8::Value> &info);
      void RequestSession(const v8::FunctionCallbackInfo<v8::Value> &info);

    private:
      static std::optional<xr::TrXRFeature> StringToXRFeature(const std::string &featureStr);
    };
  }
}