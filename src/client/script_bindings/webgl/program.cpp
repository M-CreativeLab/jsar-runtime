#include "./program.hpp"

namespace endor
{
  namespace script_bindings
  {
    namespace webgl_bindings
    {
      using namespace std;
      using namespace v8;

      void WebGLProgram::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
      {
      }

      WebGLProgram::WebGLProgram(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
          : WebGLProgramBase(isolate, args)
      {
        // WebGLProgram objects are created by WebGL context, not by user code
      }

    } // namespace webgl
  } // namespace script_bindings
} // namespace endor