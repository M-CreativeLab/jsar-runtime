#include "./renderbuffer.hpp"

namespace endor
{
  namespace script_bindings
  {
    namespace webgl_bindings
    {
      using namespace std;
      using namespace v8;

      void WebGLRenderbuffer::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
      {
      }

      WebGLRenderbuffer::WebGLRenderbuffer(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
          : WebGLRenderbufferBase(isolate, args)
      {
        // WebGLRenderbuffer objects are created by WebGL context, not by user code
      }
    } // namespace webgl
  }   // namespace script_bindings
} // namespace endor
