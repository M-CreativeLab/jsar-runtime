#include "./program.hpp"

namespace script_bindings
{
  namespace webgl_bindings
  {
    using namespace std;
    using namespace v8;

    void WebGLProgram::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
    }

    Local<Object> WebGLProgram::NewInstance(Isolate *isolate,
                                            shared_ptr<client_graphics::WebGLProgram> nativeProgram)
    {
      EscapableHandleScope scope(isolate);
      return nativeProgram != nullptr
               ? scope.Escape(WebGLProgramBase::NewInstance(isolate, nativeProgram).As<Object>())
               : scope.Escape(Local<Object>());
    }

    Local<Function> WebGLProgram::Initialize(Isolate *isolate)
    {
      return WebGLProgramBase::Initialize(isolate);
    }

    WebGLProgram::WebGLProgram(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : WebGLProgramBase(isolate, args)
    {
      // WebGLProgram objects are created by WebGL context, not by user code
    }

  } // namespace webgl
} // namespace script_bindings