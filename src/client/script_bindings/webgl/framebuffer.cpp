#include "framebuffer.hpp"
#include <v8.h>

namespace script_bindings
{
  namespace webgl
  {
    using namespace v8;

    void WebGLFramebuffer::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      tpl->SetClassName(String::NewFromUtf8(isolate, "WebGLFramebuffer").ToLocalChecked());
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      // WebGLFramebuffer objects don't have any public properties or methods
      // They are opaque handles used by the WebGL context
    }

    Local<Object> WebGLFramebuffer::NewInstance(Isolate *isolate, std::shared_ptr<webgl::WebGLFramebuffer> nativeFramebuffer)
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

      WebGLFramebuffer *wrapper = new WebGLFramebuffer(isolate, nativeFramebuffer);
      wrapper->Wrap(isolate, instance, wrapper);

      return scope.Escape(instance);
    }

    Local<Function> WebGLFramebuffer::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    WebGLFramebuffer::WebGLFramebuffer(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ObjectWrap(isolate, args)
    {
      // WebGLFramebuffer objects are created by WebGL context, not by user code
    }

  } // namespace webgl
} // namespace script_bindings