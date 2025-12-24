#pragma once

#include <memory>
#include <string>
#include <client/graphics/webgl_texture.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/webgl/object.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace webgl_bindings
    {
      class WebGLTexture;
      using WebGLTextureBase = scripting_base::ObjectWrap<WebGLTexture,
                                                          client_graphics::WebGLTexture,
                                                          WebGLObject>;

      /**
     * WebGLTexture V8 binding for texture objects.
     * 
     * This class wraps webgl::WebGLTexture objects for use in V8 JavaScript execution contexts.
     * It provides texture functionality for 2D and 3D textures.
     */
      class WebGLTexture : public WebGLTextureBase
      {
        using WebGLTextureBase::ObjectWrap;

      public:
        /**
       * The name of the WebGLTexture class for V8.
       */
        static std::string Name()
        {
          return "WebGLTexture";
        }

        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
        static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                                 std::shared_ptr<client_graphics::WebGLTexture> nativeTexture);

      public:
        WebGLTexture(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
      };

    } // namespace webgl
  }   // namespace script_bindings
} // namespace endor
