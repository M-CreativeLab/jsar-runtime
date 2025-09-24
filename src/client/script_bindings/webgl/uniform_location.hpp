#pragma once

#include <memory>
#include <string>
#include <client/graphics/webgl_uniform_location.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/webgl/object.hpp>

namespace script_bindings
{
  namespace webgl_bindings
  {
    class WebGLUniformLocation;
    using WebGLUniformLocationBase = scripting_base::ObjectWrap<WebGLUniformLocation,
                                                                client_graphics::WebGLUniformLocation,
                                                                WebGLObject>;

    class WebGLUniformLocation : public WebGLUniformLocationBase
    {
      using WebGLUniformLocationBase::ObjectWrap;

    public:
      /**
       * The name of the WebGLUniformLocation class for V8.
       */
      static std::string Name()
      {
        return "WebGLUniformLocation";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<client_graphics::WebGLUniformLocation> nativeLocation);

    public:
      WebGLUniformLocation(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };

  } // namespace webgl
} // namespace script_bindings