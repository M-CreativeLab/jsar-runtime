#pragma once

#include <map>
#include <napi.h>
#include "client/graphics/webgl_vertex_array.hpp"
#include "./object.hpp"

namespace webgl
{
  class WebGLVertexArray : public WebGLObjectBase<WebGLVertexArray, client_graphics::WebGLVertexArray>
  {
    friend class WebGLObjectBase<WebGLVertexArray, client_graphics::WebGLVertexArray>;

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
     * It creates a new instance of `WebGLVertexArray` with the given `handle`.
     *
     * @param env The environment that the instance will be created in.
     * @param handle The handle to the `WebGLVertexArray` that the instance will wrap.
     * @return The new instance of `WebGLVertexArray`.
     */
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_graphics::WebGLVertexArray> handle)
    {
      return WebGLObjectBase<WebGLVertexArray, client_graphics::WebGLVertexArray>::NewInstance(env, handle);
    }

  public:
    WebGLVertexArray(const Napi::CallbackInfo &info)
        : WebGLObjectBase<WebGLVertexArray, client_graphics::WebGLVertexArray>(info)
    {
    }

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
