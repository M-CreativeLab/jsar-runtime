#pragma once

#include <map>
#include <napi.h>
#include "client/graphics/webgl_shader.hpp"
#include "client/graphics/webgl_shader_precision_format.hpp"
#include "./object.hpp"

namespace webgl
{
  class WebGLShader : public WebGLObjectBase<WebGLShader, client_graphics::WebGLShader>
  {
    friend class WebGLObjectBase<WebGLShader, client_graphics::WebGLShader>;

  public:
    static void Init(Napi::Env env);
    /**
     * A convenient function to check whether a given JavaScript `value` is an instance of `WebGLShader`.
     * 
     * @param value The value to check.
     * @return Whether the value is an instance of `WebGLShader`.
     */
    static bool IsInstanceOf(const Napi::Value &value)
    {
      return value.As<Napi::Object>().InstanceOf(constructor->Value());
    }
    /**
     * It creates a new instance of `WebGLShader`.
     *
     * @param env The `Napi::Env` in which the instance is being created.
     * @param handle The `std::shared_ptr<client_graphics::WebGLShader>` to be used in the instance.
     */
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_graphics::WebGLShader> handle)
    {
      return WebGLObjectBase<WebGLShader, client_graphics::WebGLShader>::NewInstance(env, handle);
    }

  public:
    WebGLShader(const Napi::CallbackInfo &info)
        : WebGLObjectBase<WebGLShader, client_graphics::WebGLShader>(info)
    {
    }

  private:
    static thread_local Napi::FunctionReference *constructor;
  };

  class WebGLShaderPrecisionFormat : public Napi::ObjectWrap<WebGLShaderPrecisionFormat>
  {
  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, const client_graphics::WebGLShaderPrecisionFormat &format);

  public:
    WebGLShaderPrecisionFormat(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<WebGLShaderPrecisionFormat>(info)
    {
    }

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
