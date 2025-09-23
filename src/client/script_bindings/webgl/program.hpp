#pragma once

#include <memory>
#include <string>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/webgl/webgl_program.hpp>

namespace script_bindings
{
  namespace webgl
  {
    /**
     * WebGLProgram V8 binding for shader program objects.
     * 
     * This class wraps webgl::WebGLProgram objects for use in V8 JavaScript execution contexts.
     * It provides functionality for linked shader programs.
     */
    class WebGLProgram : public scripting_base::ObjectWrap<WebGLProgram, webgl::WebGLProgram>
    {
      using ObjectWrap::ObjectWrap;

    public:
      /**
       * The name of the WebGLProgram class for V8.
       */
      static std::string Name()
      {
        return "WebGLProgram";
      }

      /**
       * Configure the V8 function template with WebGLProgram properties.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 WebGLProgram instance from a native webgl::WebGLProgram.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<webgl::WebGLProgram> nativeProgram);

      /**
       * Initialize the WebGLProgram class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      WebGLProgram(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };

  } // namespace webgl
} // namespace script_bindings