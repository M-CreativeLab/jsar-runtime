#pragma once

#include <client/script_bindings/webgl/extension_base.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace webgl_bindings::extensions
    {
      class OES_standard_derivatives : public WebGLExtension<OES_standard_derivatives>
      {
        using WebGLExtension<OES_standard_derivatives>::WebGLExtension;

      public:
        static std::string Name()
        {
          return "OES_standard_derivatives";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
        {
          v8::HandleScope scope(isolate);
          v8::Local<v8::ObjectTemplate> instance = tpl->InstanceTemplate();

          instance->Set(isolate, "FRAGMENT_SHADER_DERIVATIVE_HINT_OES", v8::Integer::New(isolate, 0x8B8B));
        }
      };
    }
  }
} // namespace endor
