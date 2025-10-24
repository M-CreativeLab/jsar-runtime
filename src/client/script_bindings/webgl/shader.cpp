#include "shader.hpp"
#include <client/graphics/webgl_shader.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace webgl_bindings
    {
      using namespace std;
      using namespace v8;

      void WebGLShader::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
      {
        tpl->SetClassName(String::NewFromUtf8(isolate, "WebGLShader").ToLocalChecked());

        Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
        instanceTemplate->SetInternalFieldCount(1);

        // WebGLShader has no methods or properties - it's an opaque handle
      }

      Local<Object> WebGLShader::NewInstance(Isolate *isolate, shared_ptr<client_graphics::WebGLShader> nativeShader)
      {
        EscapableHandleScope scope(isolate);
        return nativeShader != nullptr
                 ? scope.Escape(WebGLShaderBase::NewInstance(isolate, nativeShader).As<Object>())
                 : scope.Escape(Local<Object>());
      }

      WebGLShader::WebGLShader(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
          : WebGLShaderBase(isolate, args)
      {
      }

      Local<Object> WebGLShaderPrecisionFormat::NewInstance(
        Isolate *isolate,
        const client_graphics::WebGLShaderPrecisionFormat &precisionFormat)
      {
        return WebGLShaderPrecisionFormatBase::NewInstance(
          isolate, make_shared<client_graphics::WebGLShaderPrecisionFormat>(precisionFormat));
      }
    }
  }
} // namespace endor
