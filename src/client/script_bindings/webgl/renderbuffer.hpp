#pragma once

#include <memory>
#include <string>
#include <client/graphics/webgl_renderbuffer.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/webgl/object.hpp>

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
      /**
       * The name of the WebGLRenderbuffer class for V8.
       */
      static std::string Name()
      {
        return "WebGLRenderbuffer";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<client_graphics::WebGLRenderbuffer> nativeRenderbuffer);
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      WebGLRenderbuffer(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };

  } // namespace webgl
} // namespace script_bindings