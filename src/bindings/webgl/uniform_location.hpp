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
    client_graphics::WebGLUniformLocation &handle() { return handle_.value(); }

  public:
    int GetValue() const { return value_; }
    void SetName(const std::string &name) { name_ = name; }
    std::string GetName() const { return name_; }

  private:
    Napi::Value ToString(const Napi::CallbackInfo &info);
    Napi::Value NameGetter(const Napi::CallbackInfo &info);
    void NameSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

  private:
    int value_;
    std::string name_;
    std::optional<client_graphics::WebGLUniformLocation> handle_;

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
