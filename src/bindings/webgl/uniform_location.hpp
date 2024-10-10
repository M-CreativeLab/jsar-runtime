#pragma once

#include <napi.h>

namespace webgl
{
  class WebGLUniformLocation : public Napi::ObjectWrap<WebGLUniformLocation>
  {
  public:
    static void Init(Napi::Env env);
    WebGLUniformLocation(const Napi::CallbackInfo &info);
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

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
