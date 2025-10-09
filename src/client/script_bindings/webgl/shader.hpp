#pragma once

#include <memory>
#include <client/graphics/webgl_shader.hpp>
#include <client/graphics/webgl_shader_precision_format.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/webgl/object.hpp>

namespace script_bindings
{
  namespace webgl_bindings
  {
    class WebGLShader;
    using WebGLShaderBase = scripting_base::ObjectWrap<WebGLShader,
                                                       client_graphics::WebGLShader,
                                                       WebGLObject>;

    /**
     * WebGLShader wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps client_graphics::WebGLShader objects for use in V8 JavaScript execution contexts.
     * It provides the standard WebGL shader interface for GPU shader objects.
     */
    class WebGLShader : public WebGLShaderBase
    {
      using WebGLShaderBase::ObjectWrap;

    public:
      /**
       * The name of the WebGLShader class for V8.
       */
      static std::string Name()
      {
        return "WebGLShader";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<client_graphics::WebGLShader> nativeShader);

    public:
      WebGLShader(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };

    class WebGLShaderPrecisionFormat;
    using WebGLShaderPrecisionFormatBase = scripting_base::ObjectWrap<WebGLShaderPrecisionFormat,
                                                                      client_graphics::WebGLShaderPrecisionFormat>;

    class WebGLShaderPrecisionFormat : public WebGLShaderPrecisionFormatBase
    {
      using WebGLShaderPrecisionFormatBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "WebGLShaderPrecisionFormat";
      }
      static v8::Local<v8::Object> NewInstance(v8::Isolate *, const client_graphics::WebGLShaderPrecisionFormat &);
    };
  }
}
