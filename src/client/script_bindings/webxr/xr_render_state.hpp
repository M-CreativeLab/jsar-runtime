#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/xr/webxr_renderstate.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace webxr_bindings
    {
      // Forward declarations
      class XRLayer;
      class XRRenderState;
      using XRRenderStateBase = scripting_base::ObjectWrap<XRRenderState, client_xr::XRRenderState>;

      /**
     * XRRenderState wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps client_xr::XRRenderState objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebXR XRRenderState interface for managing
     * rendering state including layers and depth settings.
     */
      class XRRenderState : public XRRenderStateBase
      {
        using XRRenderStateBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "XRRenderState";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      private:
        // Property getters
        void BaseLayerGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void DepthFarGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void DepthNearGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void InlineVerticalFieldOfViewGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      };
    }
  }
} // namespace endor
