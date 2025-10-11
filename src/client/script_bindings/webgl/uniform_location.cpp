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
                                                    const client_graphics::WebGLUniformLocation &loc)
    {
      EscapableHandleScope scope(isolate);
      return scope.Escape(WebGLUniformLocationBase::NewInstance(isolate,
                                                                make_shared<client_graphics::WebGLUniformLocation>(loc))
                            .As<Object>());
    }

    WebGLUniformLocation::WebGLUniformLocation(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : WebGLUniformLocationBase(isolate, args)
    {
    }

  } // namespace webgl
} // namespace script_bindings
