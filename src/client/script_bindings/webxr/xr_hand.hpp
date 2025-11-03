#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_hand.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace webxr_bindings
    {
      class XRHand;
      using XRHandBase = scripting_base::ObjectWrap<XRHand, client_xr::XRHand>;

      /**
     * XRHand wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRHand objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRHand interface for representing
     * hand tracking data with joint information.
     */
      class XRHand : public XRHandBase
      {
        using XRHandBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "XRHand";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
        static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, const client_xr::XRHand &hand);

      public:
        XRHand(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

      private:
        // Map-like methods
        void Entries(const v8::FunctionCallbackInfo<v8::Value> &info);
        void ForEach(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Get(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Keys(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Values(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Property getters
        void SizeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      };
    }
  }
} // namespace endor
