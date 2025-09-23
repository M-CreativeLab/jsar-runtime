#include "vertex_array.hpp"
#include <v8.h>

namespace script_bindings
{
  namespace webgl
  {
    using namespace v8;

    void WebGLVertexArray::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      tpl->SetClassName(String::NewFromUtf8(isolate, "WebGLVertexArray").ToLocalChecked());
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      // WebGLVertexArray objects don't have any public properties or methods
      // They are opaque handles used by the WebGL context for vertex array objects
    }

    Local<Object> WebGLVertexArray::NewInstance(Isolate *isolate, std::shared_ptr<webgl::WebGLVertexArray> nativeVertexArray)
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

      WebGLVertexArray *wrapper = new WebGLVertexArray(isolate, nativeVertexArray);
      wrapper->Wrap(isolate, instance, wrapper);

      return scope.Escape(instance);
    }

    Local<Function> WebGLVertexArray::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    WebGLVertexArray::WebGLVertexArray(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ObjectWrap(isolate, args)
    {
      // WebGLVertexArray objects are created by WebGL context, not by user code
    }

  } // namespace webgl
} // namespace script_bindings