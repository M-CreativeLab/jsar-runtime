#pragma once

#include <memory>
#include <string>
#include <client/graphics/webgl_program.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/webgl/object.hpp>

namespace endor
{
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
        static std::string Name()
        {
          return "WebGLProgram";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      public:
        WebGLProgram(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
      };
    } // namespace webgl
  } // namespace script_bindings
} // namespace endor
