#pragma once

#include <memory>
#include <string>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/webgl/webgl_placeholders.hpp>

namespace script_bindings
{
  namespace webgl
  {
    /**
     * WebGLPlaceholders V8 binding for placeholder WebGL objects.
     * 
     * This class wraps webgl::WebGLPlaceholders objects for use in V8 JavaScript execution contexts.
     * It provides placeholder objects for error handling and invalid states.
     */
    class WebGLPlaceholders : public scripting_base::ObjectWrap<WebGLPlaceholders, webgl::WebGLPlaceholders>
    {
      using ObjectWrap::ObjectWrap;

    public:
      /**
       * The name of the WebGLPlaceholders class for V8.
       */
      static std::string Name()
      {
        return "WebGLPlaceholders";
      }

      /**
       * Configure the V8 function template with WebGLPlaceholders properties.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 WebGLPlaceholders instance from a native webgl::WebGLPlaceholders.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<webgl::WebGLPlaceholders> nativePlaceholders);

      /**
       * Initialize the WebGLPlaceholders class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      WebGLPlaceholders(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Methods for creating placeholder objects
      static void CreateInvalidBufferMethod(const v8::FunctionCallbackInfo<v8::Value> &args);
      static void CreateInvalidTextureMethod(const v8::FunctionCallbackInfo<v8::Value> &args);
      static void CreateInvalidShaderMethod(const v8::FunctionCallbackInfo<v8::Value> &args);
      static void CreateInvalidProgramMethod(const v8::FunctionCallbackInfo<v8::Value> &args);
    };

  } // namespace webgl
} // namespace script_bindings