#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_viewport.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace webxr_bindings
    {
      class XRViewport;
      using XRViewportBase = scripting_base::ObjectWrap<XRViewport, client_xr::XRViewport>;

      /**
     * XRViewport wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRViewport objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRViewport interface for representing
     * the viewport within which rendering should occur.
     */
      class XRViewport : public XRViewportBase
      {
        using XRViewportBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "XRViewport";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
        static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, const client_xr::XRViewport &viewport);

      public:
        XRViewport(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

      private:
        // Property getters
        void XGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void YGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void WidthGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void HeightGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      };
    }
  }
} // namespace endor
