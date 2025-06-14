#pragma once

#include <optional>
#include <napi.h>
#include "client/graphics/webgl_uniform_location.hpp"

namespace webgl
{
  class WebGLUniformLocation : public Napi::ObjectWrap<WebGLUniformLocation>
  {
  public:
    static void Init(Napi::Env env);
    static Napi::Object NewInstance(Napi::Env env, client_graphics::WebGLUniformLocation &handle);

  public:
    WebGLUniformLocation(const Napi::CallbackInfo &info);

  public:
    /**
     * @returns the index of the uniform location.
     */
    int value() const
    {
      return handle_.has_value() ? handle_->index : -1;
    }
    /**
     * @returns the name of the uniform location.
     */
    std::string name() const
    {
      return handle_.has_value() ? handle_->name : "";
    }
    /**
     * @returns the handle of the uniform location.
     */
    client_graphics::WebGLUniformLocation &handle()
    {
      return handle_.value();
    }

  private:
    Napi::Value ToString(const Napi::CallbackInfo &info);
    Napi::Value NameGetter(const Napi::CallbackInfo &info);

  private:
    std::optional<client_graphics::WebGLUniformLocation> handle_;

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
