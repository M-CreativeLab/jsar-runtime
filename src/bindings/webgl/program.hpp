#pragma once

#include <map>
#include <napi.h>
#include "common/command_buffers/details/program.hpp"
#include "client/graphics/webgl_program.hpp"

#include "./object.hpp"
#include "./active_info.hpp"

namespace webgl
{
  class WebGLProgram : public WebGLObjectBase<WebGLProgram, client_graphics::WebGLProgram>
  {
    friend class WebGLObjectBase<WebGLProgram, client_graphics::WebGLProgram>;

  public:
    /**
     * Initialize the `WebGLProgram` class for JavaScript binding.
     */
    static void Init(Napi::Env env);
    /**
     * A convenient function to check whether a given JavaScript `value` is an instance of `WebGLTexture`.
     * 
     * @param value The value to check.
     * @return Whether the value is an instance of `WebGLTexture`.
     */
    static bool IsInstanceOf(const Napi::Value &value)
    {
      return value.IsObject() && value.As<Napi::Object>().InstanceOf(constructor->Value());
    }
    /**
     * It creates a new instance of `WebGLProgram`.
     * 
     * @param env The `Napi::Env` in which the instance is being created.
     * @param handle The `std::shared_ptr<client_graphics::WebGLProgram>` to be used in the instance.
     */
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<client_graphics::WebGLProgram> handle)
    {
      return WebGLObjectBase<WebGLProgram, client_graphics::WebGLProgram>::NewInstance(env, handle);
    }

  public:
    WebGLProgram(const Napi::CallbackInfo &info);

  public:
    void printInfo()
    {
      handle_->printInfo();
    }

  private:
    Napi::Value ToString(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
