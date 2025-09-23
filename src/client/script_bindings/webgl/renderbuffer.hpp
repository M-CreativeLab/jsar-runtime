#pragma once

#include <memory>
#include <string>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/webgl/webgl_renderbuffer.hpp>

namespace script_bindings
{
  namespace webgl
  {
    /**
     * WebGLRenderbuffer V8 binding for renderbuffer objects.
     * 
     * This class wraps webgl::WebGLRenderbuffer objects for use in V8 JavaScript execution contexts.
     * It provides renderbuffer functionality for depth and stencil buffers.
     */
    class WebGLRenderbuffer : public scripting_base::ObjectWrap<WebGLRenderbuffer, webgl::WebGLRenderbuffer>
    {
      using ObjectWrap::ObjectWrap;

    public:
      /**
       * The name of the WebGLRenderbuffer class for V8.
       */
      static std::string Name()
      {
        return "WebGLRenderbuffer";
      }

      /**
       * Configure the V8 function template with WebGLRenderbuffer properties.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 WebGLRenderbuffer instance from a native webgl::WebGLRenderbuffer.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<webgl::WebGLRenderbuffer> nativeRenderbuffer);

      /**
       * Initialize the WebGLRenderbuffer class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      WebGLRenderbuffer(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };

  } // namespace webgl
} // namespace script_bindings