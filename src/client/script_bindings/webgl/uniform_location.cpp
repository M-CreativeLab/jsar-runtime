#include "uniform_location.hpp"
#include <v8.h>

namespace script_bindings
{
  namespace webgl_bindings
  {
    using namespace std;
    using namespace v8;

    void WebGLUniformLocation::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      // WebGLUniformLocation objects don't have any public properties or methods
      // They are opaque handles used by the WebGL context for uniform locations
    }

    Local<Object> WebGLUniformLocation::NewInstance(Isolate *isolate,
                                                    shared_ptr<client_graphics::WebGLUniformLocation> nativeLocation)
    {
      EscapableHandleScope scope(isolate);
      return nativeLocation != nullptr
               ? scope.Escape(WebGLUniformLocationBase::NewInstance(isolate, nativeLocation).As<Object>())
               : scope.Escape(Local<Object>());
    }

    WebGLUniformLocation::WebGLUniformLocation(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : WebGLUniformLocationBase(isolate, args)
    {
    }

  } // namespace webgl
} // namespace script_bindings
