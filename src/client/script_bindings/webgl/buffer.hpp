#pragma once

#include <memory>
#include <client/graphics/webgl_buffer.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/webgl/object.hpp>

namespace script_bindings
{
  namespace webgl_bindings
  {
    class WebGLBuffer;
    using WebGLBufferBase = scripting_base::ObjectWrap<WebGLBuffer,
                                                       client_graphics::WebGLBuffer,
                                                       WebGLObject>;

    /**
     * WebGLBuffer wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps client_graphics::WebGLBuffer objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebGL buffer interface for GPU buffer objects.
     */
    class WebGLBuffer : public WebGLBufferBase
    {
      using WebGLBufferBase::ObjectWrap;

    public:
      /**
       * The name of the WebGLBuffer class for V8.
       */
      static std::string Name()
      {
        return "WebGLBuffer";
      }

      /**
       * Configure the V8 function template with WebGLBuffer properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 WebGLBuffer instance from a native client_graphics::WebGLBuffer.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<client_graphics::WebGLBuffer> nativeBuffer);

      /**
       * Check if a V8 value is an instance of WebGLBuffer.
       */
      static bool IsInstanceOf(v8::Isolate *isolate, v8::Local<v8::Value> value);

      /**
       * Initialize the WebGLBuffer class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      WebGLBuffer(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}