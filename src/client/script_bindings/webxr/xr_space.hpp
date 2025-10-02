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
      using XRSpaceBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "XRSpace";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
    };

    class XRReferenceSpace;
    using XRReferenceSpaceBase = scripting_base::ObjectWrap<XRReferenceSpace, client_xr::XRReferenceSpace, XRSpace>;

    /**
     * XRReferenceSpace wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRReferenceSpace objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRReferenceSpace interface.
     */
    class XRReferenceSpace : public XRReferenceSpaceBase
    {
      using XRReferenceSpaceBase::ObjectWrap;

    public:
      /**
       * The name of the XRReferenceSpace class for V8.
       */
      static std::string Name()
      {
        return "XRReferenceSpace";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    private:
      // Methods
      void GetOffsetReferenceSpace(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}
