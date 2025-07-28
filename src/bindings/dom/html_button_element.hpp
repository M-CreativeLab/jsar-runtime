#pragma once

#include <client/html/html_button_element.hpp>
#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLButtonElement : public HTMLElementBase<HTMLButtonElement, dom::HTMLButtonElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static void Init(Napi::Env env);
    static std::vector<Napi::ClassPropertyDescriptor<HTMLButtonElement>> GetClassProperties(Napi::Env env);

  public:
    static thread_local Napi::FunctionReference *constructor;

  private:
    // Property getters/setters
    Napi::Value GetDisabled(const Napi::CallbackInfo &info);
    void SetDisabled(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value GetType(const Napi::CallbackInfo &info);
    void SetType(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value GetForm(const Napi::CallbackInfo &info);
    void SetForm(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value GetName(const Napi::CallbackInfo &info);
    void SetName(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value GetValue(const Napi::CallbackInfo &info);
    void SetValue(const Napi::CallbackInfo &info, const Napi::Value &value);
  };
}
