#pragma once

#include <memory>
#include <string>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/webgl/webgl_texture.hpp>

namespace script_bindings
{
  namespace webgl
  {
    /**
     * WebGLTexture V8 binding for texture objects.
     * 
     * This class wraps webgl::WebGLTexture objects for use in V8 JavaScript execution contexts.
     * It provides texture functionality for 2D and 3D textures.
     */
    class WebGLTexture : public scripting_base::ObjectWrap<WebGLTexture, webgl::WebGLTexture>
    {
      using ObjectWrap::ObjectWrap;

    public:
      /**
       * The name of the WebGLTexture class for V8.
       */
      static std::string Name()
      {
        return "WebGLTexture";
      }

      /**
       * Configure the V8 function template with WebGLTexture properties.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 WebGLTexture instance from a native webgl::WebGLTexture.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<webgl::WebGLTexture> nativeTexture);

      /**
       * Initialize the WebGLTexture class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      WebGLTexture(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };

  } // namespace webgl
} // namespace script_bindings