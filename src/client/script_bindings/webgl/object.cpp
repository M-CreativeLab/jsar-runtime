#include "object.hpp"
#include <v8.h>

namespace script_bindings
{
  namespace webgl
  {
    using namespace v8;

    void WebGLObject::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      tpl->SetClassName(String::NewFromUtf8(isolate, "WebGLObject").ToLocalChecked());
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      // WebGLObject is the base class for all WebGL objects
      // It doesn't have any public properties or methods in the WebGL specification
    }

    Local<Object> WebGLObject::NewInstance(Isolate *isolate, std::shared_ptr<webgl::WebGLObject> nativeObject)
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

      WebGLObject *wrapper = new WebGLObject(isolate, nativeObject);
      wrapper->Wrap(isolate, instance, wrapper);

      return scope.Escape(instance);
    }

    Local<Function> WebGLObject::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    WebGLObject::WebGLObject(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ObjectWrap(isolate, args)
    {
      // WebGLObject is a base class, typically not instantiated directly
    }

  } // namespace webgl
} // namespace script_bindings