#pragma once

#include <memory>
#include <string>
#include <client/graphics/webgl_framebuffer.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/webgl/object.hpp>

namespace script_bindings
{
  namespace webgl_bindings
  {
    class WebGLFramebuffer;
    using WebGLFramebufferBase = scripting_base::ObjectWrap<WebGLFramebuffer,
                                                            client_graphics::WebGLFramebuffer,
                                                            WebGLObject>;

    class WebGLFramebuffer : public WebGLFramebufferBase
    {
      using WebGLFramebufferBase::ObjectWrap;

    public:
      /**
       * The name of the WebGLFramebuffer class for V8.
       */
      static std::string Name()
      {
        return "WebGLFramebuffer";
      }

      /**
       * Configure the V8 function template with WebGLFramebuffer properties.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 WebGLFramebuffer instance from a native webgl::WebGLFramebuffer.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<client_graphics::WebGLFramebuffer> nativeFramebuffer);

      /**
       * Initialize the WebGLFramebuffer class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      WebGLFramebuffer(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };

  } // namespace webgl
} // namespace script_bindings