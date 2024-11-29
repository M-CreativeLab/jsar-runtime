#pragma once

#include <map>
#include <napi.h>
#include "client/graphics/webgl_texture.hpp"
#include "./object.hpp"

namespace webgl
{
  class WebGLTexture : public WebGLObjectBase<WebGLTexture, client_graphics::WebGLTexture>
  {
    friend class WebGLObjectBase<WebGLTexture, client_graphics::WebGLTexture>;

  public:
    static void Init(Napi::Env env);
    /**
     * A convenient function to check whether a given JavaScript `value` is an instance of `WebGLTexture`.
     * 
     * @param value The value to check.
     * @return Whether the value is an instance of `WebGLTexture`.
     */
    static bool IsInstanceOf(const Napi::Value &value)
    {
      return value.As<Napi::Object>().InstanceOf(constructor->Value());
    }
    /**
     * It creates a new instance of `WebGLTexture` with the given `handle`.
     * 
     * @param env The environment that the instance will be created in.
     * @param handle The handle to the `WebGLTexture` that the instance will wrap.
     * @return The new instance of `WebGLTexture`.
     */
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_graphics::WebGLTexture> handle)
    {
      return WebGLObjectBase<WebGLTexture, client_graphics::WebGLTexture>::NewInstance(env, handle);
    }

  public:
    WebGLTexture(const Napi::CallbackInfo &info)
        : WebGLObjectBase<WebGLTexture, client_graphics::WebGLTexture>(info)
    {
    }

  public:
    uint32_t GetId() const { return handle_->id; }

  private:
    Napi::Value ToString(const Napi::CallbackInfo &info);
    Napi::Value GetValue(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
