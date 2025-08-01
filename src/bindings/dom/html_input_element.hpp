#pragma once

#include <client/html/html_input_element.hpp>
#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLInputElement : public HTMLElementBase<HTMLInputElement, dom::HTMLInputElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static std::vector<Napi::ClassPropertyDescriptor<HTMLInputElement>> GetClassProperties(Napi::Env env);
    static void Init(Napi::Env env);

  private:
    // Property getters and setters
    Napi::Value TypeGetter(const Napi::CallbackInfo &info);
    void TypeSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

    Napi::Value ValueGetter(const Napi::CallbackInfo &info);
    void ValueSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

    Napi::Value CheckedGetter(const Napi::CallbackInfo &info);
    void CheckedSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

    Napi::Value DisabledGetter(const Napi::CallbackInfo &info);
    void DisabledSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

    Napi::Value RequiredGetter(const Napi::CallbackInfo &info);
    void RequiredSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

    Napi::Value PlaceholderGetter(const Napi::CallbackInfo &info);
    void PlaceholderSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

    Napi::Value NameGetter(const Napi::CallbackInfo &info);
    void NameSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

    Napi::Value ValueAsNumberGetter(const Napi::CallbackInfo &info);
    void ValueAsNumberSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

  private:
    // Methods
    Napi::Value CheckValidity(const Napi::CallbackInfo &info);
    Napi::Value SetCustomValidity(const Napi::CallbackInfo &info);
    Napi::Value ReportValidity(const Napi::CallbackInfo &info);
    Napi::Value StepUp(const Napi::CallbackInfo &info);
    Napi::Value StepDown(const Napi::CallbackInfo &info);
    Napi::Value Select(const Napi::CallbackInfo &info);
    Napi::Value SetSelectionRange(const Napi::CallbackInfo &info);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
