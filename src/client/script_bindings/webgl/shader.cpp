#include "shader.hpp"
#include <client/graphics/webgl_shader.hpp>

namespace script_bindings
{
  namespace webgl
  {
    using namespace v8;

    void WebGLShader::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      tpl->SetClassName(String::NewFromUtf8(isolate, "WebGLShader").ToLocalChecked());

      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
      instanceTemplate->SetInternalFieldCount(1);

      // WebGLShader has no methods or properties - it's an opaque handle
    }

    Local<Object> WebGLShader::NewInstance(Isolate *isolate, std::shared_ptr<client_graphics::WebGLShader> nativeShader)
    {
      Local<Function> constructor = GetConstructorFunction(isolate);
      Local<Context> context = isolate->GetCurrentContext();
      Local<Object> instance = constructor->NewInstance(context).ToLocalChecked();

      WebGLShader *shaderWrapper = ObjectWrap::Unwrap<WebGLShader>(instance);
      shaderWrapper->SetNativeInstance(nativeShader);

      return instance;
    }

    bool WebGLShader::IsInstanceOf(Isolate *isolate, Local<Value> value)
    {
      if (!value->IsObject())
        return false;

      Local<Object> obj = value.As<Object>();
      return ObjectWrap::HasInstance(isolate, obj);
    }

    Local<Function> WebGLShader::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    WebGLShader::WebGLShader(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : WebGLShaderBase(isolate, args)
    {
      // WebGLShader should not be directly constructible by JavaScript
      // It's created internally by WebGL context methods
      isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "WebGLShader cannot be constructed directly").ToLocalChecked()));
    }
  }
}