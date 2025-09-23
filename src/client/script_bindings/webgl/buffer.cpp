#include "buffer.hpp"
#include <client/graphics/webgl_buffer.hpp>

namespace script_bindings
{
  namespace webgl
  {
    using namespace v8;

    void WebGLBuffer::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      tpl->SetClassName(String::NewFromUtf8(isolate, "WebGLBuffer").ToLocalChecked());

      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
      instanceTemplate->SetInternalFieldCount(1);

      // WebGLBuffer has no methods or properties - it's an opaque handle
    }

    Local<Object> WebGLBuffer::NewInstance(Isolate *isolate, std::shared_ptr<client_graphics::WebGLBuffer> nativeBuffer)
    {
      Local<Function> constructor = GetConstructorFunction(isolate);
      Local<Context> context = isolate->GetCurrentContext();
      Local<Object> instance = constructor->NewInstance(context).ToLocalChecked();

      WebGLBuffer *bufferWrapper = ObjectWrap::Unwrap<WebGLBuffer>(instance);
      bufferWrapper->SetNativeInstance(nativeBuffer);

      return instance;
    }

    bool WebGLBuffer::IsInstanceOf(Isolate *isolate, Local<Value> value)
    {
      if (!value->IsObject())
        return false;

      Local<Object> obj = value.As<Object>();
      return ObjectWrap::HasInstance(isolate, obj);
    }

    Local<Function> WebGLBuffer::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    WebGLBuffer::WebGLBuffer(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : WebGLBufferBase(isolate, args)
    {
      // WebGLBuffer should not be directly constructible by JavaScript
      // It's created internally by WebGL context methods
      isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "WebGLBuffer cannot be constructed directly").ToLocalChecked()));
    }
  }
}