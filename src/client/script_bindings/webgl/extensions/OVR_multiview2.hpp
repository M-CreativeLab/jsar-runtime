#pragma once

#include <client/script_bindings/webgl/extension_base.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace webgl_bindings::extensions
    {
      class OVR_multiview2 : public WebGLExtension<OVR_multiview2>
      {
        using WebGLExtension<OVR_multiview2>::WebGLExtension;

      public:
        static std::string Name()
        {
          return "OVR_multiview2";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      private:
        void FramebufferTextureMultiviewOVR(const v8::FunctionCallbackInfo<v8::Value> &args);
      };
    }
  }
} // namespace endor
