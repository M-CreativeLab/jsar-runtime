#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/graphics/webgl_shader.hpp>

namespace script_bindings
{
  namespace webgl
  {
    class WebGLShader;
    using WebGLShaderBase = scripting_base::ObjectWrap<WebGLShader, client_graphics::WebGLShader>;

    /**
     * WebGLShader wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps client_graphics::WebGLShader objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebGL shader interface for GPU shader objects.
     */
    class WebGLShader : public WebGLShaderBase
    {
      using WebGLShaderBase::ObjectWrap;

    public:
      /**
       * The name of the WebGLShader class for V8.
       */
      static std::string Name()
      {
        return "WebGLShader";
      }

      /**
       * Configure the V8 function template with WebGLShader properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 WebGLShader instance from a native client_graphics::WebGLShader.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_graphics::WebGLShader> nativeShader);

      /**
       * Check if a V8 value is an instance of WebGLShader.
       */
      static bool IsInstanceOf(v8::Isolate *isolate, v8::Local<v8::Value> value);

      /**
       * Initialize the WebGLShader class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      WebGLShader(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}