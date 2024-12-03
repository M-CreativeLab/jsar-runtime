#pragma once

#include <napi.h>
#include "client/graphics/webgl_buffer.hpp"
#include "./object.hpp"

namespace webgl
{
  class WebGLBuffer : public WebGLObjectBase<WebGLBuffer, client_graphics::WebGLBuffer>
  {
    friend class WebGLObjectBase<WebGLBuffer, client_graphics::WebGLBuffer>;

  public:
    /**
     * Initialize `WebGLBuffer` for JavaScript runtime.
     */
    static void Init(Napi::Env env);
    /**
     * A convenient function to check whether a given JavaScript `value` is an instance of `WebGLBuffer`.
     * 
     * @param value The value to check.
     * @return Whether the value is an instance of `WebGLBuffer`.
     */
    static bool IsInstanceOf(const Napi::Value &value)
    {
      return value.As<Napi::Object>().InstanceOf(constructor->Value());
    }
    /**
     * Create a new instance of `WebGLBuffer` with the given native handle.
     * 
     * @param env The environment that the instance will be created in.
     * @param handle The native handle of the instance.
     */
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_graphics::WebGLBuffer> handle)
    {
      return WebGLObjectBase<WebGLBuffer, client_graphics::WebGLBuffer>::NewInstance(env, handle);
    }

  public:
    WebGLBuffer(const Napi::CallbackInfo &info)
        : WebGLObjectBase<WebGLBuffer, client_graphics::WebGLBuffer>(info)
    {
    }

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
