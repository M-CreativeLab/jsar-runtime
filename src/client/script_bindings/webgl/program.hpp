#pragma once

#include <memory>
#include <string>
#include <client/graphics/webgl_program.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/webgl/object.hpp>

namespace script_bindings
{
  namespace webgl_bindings
  {
    class WebGLProgram;
    using WebGLProgramBase = scripting_base::ObjectWrap<WebGLProgram,
                                                        client_graphics::WebGLProgram,
                                                        WebGLObject>;

    class WebGLProgram : public WebGLProgramBase
    {
      using WebGLProgramBase::ObjectWrap;

    public:
      /**
       * The name of the WebGLProgram class for V8.
       */
      static std::string Name()
      {
        return "WebGLProgram";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<client_graphics::WebGLProgram> nativeProgram);
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      WebGLProgram(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  } // namespace webgl
} // namespace script_bindings
