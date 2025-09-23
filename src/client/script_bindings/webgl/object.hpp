#pragma once

#include <memory>
#include <string>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/webgl/webgl_object.hpp>

namespace script_bindings
{
  namespace webgl
  {
    /**
     * WebGLObject V8 binding for base WebGL object functionality.
     * 
     * This class wraps webgl::WebGLObject objects for use in V8 JavaScript execution contexts.
     * It provides the base class for all WebGL objects.
     */
    class WebGLObject : public scripting_base::ObjectWrap<WebGLObject, webgl::WebGLObject>
    {
      using ObjectWrap::ObjectWrap;

    public:
      /**
       * The name of the WebGLObject class for V8.
       */
      static std::string Name()
      {
        return "WebGLObject";
      }

      /**
       * Configure the V8 function template with WebGLObject properties.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 WebGLObject instance from a native webgl::WebGLObject.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<webgl::WebGLObject> nativeObject);

      /**
       * Initialize the WebGLObject class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      WebGLObject(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };

  } // namespace webgl
} // namespace script_bindings