#include "buffer.hpp"
#include <client/graphics/webgl_buffer.hpp>

namespace script_bindings
{
  namespace webgl_bindings
  {
    using namespace std;
    using namespace v8;

    void WebGLBuffer::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
    }

    Local<Object> WebGLBuffer::NewInstance(Isolate *isolate, shared_ptr<client_graphics::WebGLBuffer> nativeBuffer)
    {
      EscapableHandleScope scope(isolate);
      return nativeBuffer != nullptr
               ? scope.Escape(WebGLBufferBase::NewInstance(isolate, nativeBuffer).As<Object>())
               : scope.Escape(Local<Object>());
    }

    WebGLBuffer::WebGLBuffer(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : WebGLBufferBase(isolate, args)
    {
    }
  }
}
