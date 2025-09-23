#include "renderbuffer.hpp"
#include <v8.h>

namespace script_bindings
{
  namespace webgl
  {
    using namespace v8;

    void WebGLRenderbuffer::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      tpl->SetClassName(String::NewFromUtf8(isolate, "WebGLRenderbuffer").ToLocalChecked());
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      // WebGLRenderbuffer objects don't have any public properties or methods
      // They are opaque handles used by the WebGL context for render buffers
    }

    Local<Object> WebGLRenderbuffer::NewInstance(Isolate *isolate, std::shared_ptr<webgl::WebGLRenderbuffer> nativeRenderbuffer)
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

      WebGLRenderbuffer *wrapper = new WebGLRenderbuffer(isolate, nativeRenderbuffer);
      wrapper->Wrap(isolate, instance, wrapper);

      return scope.Escape(instance);
    }

    Local<Function> WebGLRenderbuffer::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    WebGLRenderbuffer::WebGLRenderbuffer(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ObjectWrap(isolate, args)
    {
      // WebGLRenderbuffer objects are created by WebGL context, not by user code
    }

  } // namespace webgl
} // namespace script_bindings