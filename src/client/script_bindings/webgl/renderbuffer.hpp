#pragma once

#include <memory>
#include <string>
#include <client/graphics/webgl_renderbuffer.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/webgl/object.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace webgl_bindings
    {
      class WebGLRenderbuffer;
      using WebGLRenderbufferBase = scripting_base::ObjectWrap<WebGLRenderbuffer,
                                                               client_graphics::WebGLRenderbuffer,
                                                               WebGLObject>;

      class WebGLRenderbuffer : public WebGLRenderbufferBase
      {
        using WebGLRenderbufferBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "WebGLRenderbuffer";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      public:
        WebGLRenderbuffer(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
      };

    } // namespace webgl
  }   // namespace script_bindings
} // namespace endor