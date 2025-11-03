#include "framebuffer.hpp"

namespace endor
{
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
        assert(nativeFramebuffer != nullptr && "nativeFramebuffer must not be null");
        return scope.Escape(WebGLFramebufferBase::NewInstance(isolate, nativeFramebuffer).As<Object>());
      }

      Local<Object> WebGLFramebuffer::NewInstance(Isolate *isolate, uint32_t framebufferId)
      {
        return NewInstance(isolate, make_shared<client_graphics::WebGLFramebuffer>(framebufferId));
      }

      WebGLFramebuffer::WebGLFramebuffer(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
          : WebGLFramebufferBase(isolate, args)
      {
      }
    } // namespace webgl
  }   // namespace script_bindings
} // namespace endor