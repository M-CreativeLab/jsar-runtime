#pragma once

#include <napi.h>
#include "client/graphics/webgl_renderbuffer.hpp"
#include "./object.hpp"

namespace webgl
{
  class WebGLRenderbuffer : public WebGLObjectBase<WebGLRenderbuffer, client_graphics::WebGLRenderbuffer>
  {
    friend class WebGLObjectBase<WebGLRenderbuffer, client_graphics::WebGLRenderbuffer>;

  public:
    static void Init(Napi::Env env);
    /**
     * A convenient function to check whether a given JavaScript `value` is an instance of `WebGLRenderbuffer`.
     * 
     * @param value The value to check.
     * @return Whether the value is an instance of `WebGLRenderbuffer`.
     */
    static bool IsInstanceOf(const Napi::Value &value)
    {
      return value.As<Napi::Object>().InstanceOf(constructor->Value());
    }
    /**
     * Create a new instance of `WebGLRenderbuffer`.
     *
     * @param env The environment that the instance will be created in.
     * @param handle The native handle of the instance.
     * @returns The new instance of `WebGLRenderbuffer`.
     */
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_graphics::WebGLRenderbuffer> handle)
    {
      return WebGLObjectBase<WebGLRenderbuffer, client_graphics::WebGLRenderbuffer>::NewInstance(env, handle);
    }

  public:
    WebGLRenderbuffer(const Napi::CallbackInfo &info)
        : WebGLObjectBase<WebGLRenderbuffer, client_graphics::WebGLRenderbuffer>(info)
    {
    }

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
