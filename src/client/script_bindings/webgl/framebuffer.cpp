#include "framebuffer.hpp"

namespace script_bindings
{
  namespace webgl_bindings
  {
    using namespace std;
    using namespace v8;

    void WebGLFramebuffer::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
    }

    Local<Object> WebGLFramebuffer::NewInstance(Isolate *isolate,
                                                std::shared_ptr<client_graphics::WebGLFramebuffer> nativeFramebuffer)
    {
      EscapableHandleScope scope(isolate);
      return nativeFramebuffer != nullptr
               ? scope.Escape(WebGLFramebufferBase::NewInstance(isolate, nativeFramebuffer).As<Object>())
               : scope.Escape(Local<Object>());
    }

    Local<Function> WebGLFramebuffer::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    WebGLFramebuffer::WebGLFramebuffer(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : WebGLFramebufferBase(isolate, args)
    {
    }

  } // namespace webgl
} // namespace script_bindings