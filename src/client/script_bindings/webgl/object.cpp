#include "object.hpp"
#include <v8.h>

namespace script_bindings
{
  namespace webgl_bindings
  {
    using namespace std;
    using namespace v8;

    void WebGLObject::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
    }

    Local<Object> WebGLObject::NewInstance(Isolate *isolate, shared_ptr<client_graphics::WebGLObject> nativeObject)
    {
      EscapableHandleScope scope(isolate);
      return nativeObject != nullptr
               ? scope.Escape(WebGLObjectBase::NewInstance(isolate, nativeObject).As<Object>())
               : scope.Escape(Local<Object>());
    }

    WebGLObject::WebGLObject(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : WebGLObjectBase(isolate, args)
    {
      // WebGLObject is a base class, typically not instantiated directly
    }
  } // namespace webgl
} // namespace script_bindings
