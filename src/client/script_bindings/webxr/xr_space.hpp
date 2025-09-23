#pragma once

#include <memory>
#include <client/xr/webxr_spaces.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/event_target.hpp>

namespace script_bindings
{
  namespace webxr_bindings
  {
    class XRSpace;
    using XRSpaceBase = scripting_base::ObjectWrap<XRSpace, client_xr::XRSpace, EventTarget>;

    class XRSpace : public XRSpaceBase
    {
    public:
      /**
       * The name of the XRSpace class for V8.
       */
      static std::string Name()
      {
        return "XRSpace";
      }

      /**
       * Configure the V8 function template with XRSpace properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 XRSpace instance from a native client_xr::XRSpace.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRSpace> nativeSpace);

      /**
       * Initialize the XRSpace class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      XRSpace(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };

    /**
     * XRReferenceSpace wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRReferenceSpace objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRReferenceSpace interface.
     */
    class XRReferenceSpace : public scripting_base::ObjectWrap<XRReferenceSpace,
                                                               client_xr::XRReferenceSpace,
                                                               XRSpace>
    {
    public:
      /**
       * The name of the XRReferenceSpace class for V8.
       */
      static std::string Name()
      {
        return "XRReferenceSpace";
      }

      /**
       * Configure the V8 function template with XRReferenceSpace properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 XRReferenceSpace instance from a native client_xr::XRReferenceSpace.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_xr::XRReferenceSpace> nativeSpace);

      /**
       * Initialize the XRReferenceSpace class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      XRReferenceSpace(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Methods
      static void GetOffsetReferenceSpace(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}