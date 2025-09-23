#pragma once

#include <memory>
#include <string>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/webgl/webgl_uniform_location.hpp>

namespace script_bindings
{
  namespace webgl
  {
    /**
     * WebGLUniformLocation V8 binding for uniform location objects.
     * 
     * This class wraps webgl::WebGLUniformLocation objects for use in V8 JavaScript execution contexts.
     * It provides uniform location functionality for shader uniforms.
     */
    class WebGLUniformLocation : public scripting_base::ObjectWrap<WebGLUniformLocation, webgl::WebGLUniformLocation>
    {
      using ObjectWrap::ObjectWrap;

    public:
      /**
       * The name of the WebGLUniformLocation class for V8.
       */
      static std::string Name()
      {
        return "WebGLUniformLocation";
      }

      /**
       * Configure the V8 function template with WebGLUniformLocation properties.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 WebGLUniformLocation instance from a native webgl::WebGLUniformLocation.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<webgl::WebGLUniformLocation> nativeLocation);

      /**
       * Initialize the WebGLUniformLocation class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      WebGLUniformLocation(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };

  } // namespace webgl
} // namespace script_bindings