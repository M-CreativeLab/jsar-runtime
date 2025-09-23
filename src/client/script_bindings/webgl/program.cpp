#include "program.hpp"
#include <v8.h>

namespace script_bindings
{
  namespace webgl
  {
    using namespace v8;

    void WebGLProgram::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      tpl->SetClassName(String::NewFromUtf8(isolate, "WebGLProgram").ToLocalChecked());
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      // WebGLProgram objects don't have any public properties or methods
      // They are opaque handles used by the WebGL context for shader programs
    }

    Local<Object> WebGLProgram::NewInstance(Isolate *isolate, std::shared_ptr<webgl::WebGLProgram> nativeProgram)
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

      WebGLProgram *wrapper = new WebGLProgram(isolate, nativeProgram);
      wrapper->Wrap(isolate, instance, wrapper);

      return scope.Escape(instance);
    }

    Local<Function> WebGLProgram::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    WebGLProgram::WebGLProgram(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ObjectWrap(isolate, args)
    {
      // WebGLProgram objects are created by WebGL context, not by user code
    }

  } // namespace webgl
} // namespace script_bindings