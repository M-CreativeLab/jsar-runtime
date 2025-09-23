#include "texture.hpp"
#include <v8.h>

namespace script_bindings
{
  namespace webgl
  {
    using namespace v8;

    void WebGLTexture::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      tpl->SetClassName(String::NewFromUtf8(isolate, "WebGLTexture").ToLocalChecked());
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      // WebGLTexture objects don't have any public properties or methods
      // They are opaque handles used by the WebGL context for textures
    }

    Local<Object> WebGLTexture::NewInstance(Isolate *isolate, std::shared_ptr<webgl::WebGLTexture> nativeTexture)
    {
      EscapableHandleScope scope(isolate);
      Local<Function> constructor = GetConstructorFunction(isolate);
      if (constructor.IsEmpty())
      {
        return scope.Escape(Local<Object>());
      }

      Local<Context> context = isolate->GetCurrentContext();
      Local<Object> instance;
      if (!constructor->NewInstance(context, 0, nullptr).ToLocal(&instance))
      {
        return scope.Escape(Local<Object>());
      }

      WebGLTexture *wrapper = new WebGLTexture(isolate, nativeTexture);
      wrapper->Wrap(isolate, instance, wrapper);

      return scope.Escape(instance);
    }

    Local<Function> WebGLTexture::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    WebGLTexture::WebGLTexture(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ObjectWrap(isolate, args)
    {
      // WebGLTexture objects are created by WebGL context, not by user code
    }

  } // namespace webgl
} // namespace script_bindings