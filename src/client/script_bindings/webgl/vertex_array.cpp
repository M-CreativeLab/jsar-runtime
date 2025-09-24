#include "./vertex_array.hpp"

namespace script_bindings
{
  namespace webgl_bindings
  {
    using namespace std;
    using namespace v8;

    void WebGLVertexArray::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      // WebGLVertexArray objects don't have any public properties or methods
      // They are opaque handles used by the WebGL context for vertex array objects
    }

    Local<Object> WebGLVertexArray::NewInstance(Isolate *isolate,
                                                shared_ptr<client_graphics::WebGLVertexArray> nativeVertexArray)
    {
      EscapableHandleScope scope(isolate);
      return nativeVertexArray != nullptr
               ? scope.Escape(WebGLVertexArrayBase::NewInstance(isolate, nativeVertexArray).As<Object>())
               : scope.Escape(Local<Object>());
    }

    WebGLVertexArray::WebGLVertexArray(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : WebGLVertexArrayBase(isolate, args)
    {
      // WebGLVertexArray objects are created by WebGL context, not by user code
    }

  } // namespace webgl
} // namespace script_bindings
