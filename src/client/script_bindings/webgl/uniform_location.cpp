#include "uniform_location.hpp"
#include <v8.h>

namespace script_bindings
{
  namespace webgl
  {
    using namespace v8;

    void WebGLUniformLocation::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      tpl->SetClassName(String::NewFromUtf8(isolate, "WebGLUniformLocation").ToLocalChecked());
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      // WebGLUniformLocation objects don't have any public properties or methods
      // They are opaque handles used by the WebGL context for uniform locations
    }

    Local<Object> WebGLUniformLocation::NewInstance(Isolate *isolate, std::shared_ptr<webgl::WebGLUniformLocation> nativeLocation)
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

      WebGLUniformLocation *wrapper = new WebGLUniformLocation(isolate, nativeLocation);
      wrapper->Wrap(isolate, instance, wrapper);

      return scope.Escape(instance);
    }

    Local<Function> WebGLUniformLocation::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    WebGLUniformLocation::WebGLUniformLocation(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ObjectWrap(isolate, args)
    {
      // WebGLUniformLocation objects are created by WebGL context, not by user code
    }

  } // namespace webgl
} // namespace script_bindings