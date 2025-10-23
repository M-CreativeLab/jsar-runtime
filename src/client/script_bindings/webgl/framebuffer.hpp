#pragma once

#include <memory>
#include <string>
#include <client/graphics/webgl_framebuffer.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/webgl/object.hpp>

namespace endor
{
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
        static std::string Name()
        {
          return "WebGLFramebuffer";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

        static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                                 std::shared_ptr<client_graphics::WebGLFramebuffer> nativeFramebuffer);
        static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, uint32_t framebufferId);

      public:
        WebGLFramebuffer(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
      };

    } // namespace webgl
  } // namespace script_bindings
} // namespace endor
