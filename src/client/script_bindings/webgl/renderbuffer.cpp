#include "./renderbuffer.hpp"

namespace script_bindings
{
  namespace webgl_bindings
  {
    using namespace std;
    using namespace v8;

    void WebGLRenderbuffer::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
    }

    Local<Object> WebGLRenderbuffer::NewInstance(Isolate *isolate,
                                                 std::shared_ptr<client_graphics::WebGLRenderbuffer> nativeRenderbuffer)
    {
      EscapableHandleScope scope(isolate);
      return nativeRenderbuffer != nullptr
               ? scope.Escape(WebGLRenderbufferBase::NewInstance(isolate, nativeRenderbuffer).As<Object>())
               : scope.Escape(Local<Object>());
    }

    Local<Function> WebGLRenderbuffer::Initialize(Isolate *isolate)
    {
      return WebGLRenderbufferBase::Initialize(isolate);
    }

    WebGLRenderbuffer::WebGLRenderbuffer(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : WebGLRenderbufferBase(isolate, args)
    {
      // WebGLRenderbuffer objects are created by WebGL context, not by user code
    }

  } // namespace webgl
} // namespace script_bindings