#pragma once

#include <memory>
#include <string>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/webgl/webgl_vertex_array.hpp>

namespace script_bindings
{
  namespace webgl
  {
    /**
     * WebGLVertexArray V8 binding for vertex array objects.
     * 
     * This class wraps webgl::WebGLVertexArray objects for use in V8 JavaScript execution contexts.
     * It provides vertex array functionality for efficient rendering state management.
     */
    class WebGLVertexArray : public scripting_base::ObjectWrap<WebGLVertexArray, webgl::WebGLVertexArray>
    {
      using ObjectWrap::ObjectWrap;

    public:
      /**
       * The name of the WebGLVertexArray class for V8.
       */
      static std::string Name()
      {
        return "WebGLVertexArray";
      }

      /**
       * Configure the V8 function template with WebGLVertexArray properties.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 WebGLVertexArray instance from a native webgl::WebGLVertexArray.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<webgl::WebGLVertexArray> nativeVertexArray);

      /**
       * Initialize the WebGLVertexArray class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      WebGLVertexArray(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };

  } // namespace webgl
} // namespace script_bindings