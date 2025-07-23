#pragma once

#include <client/html/html_model_element.hpp>

#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLModelElement : public HTMLElementBase<HTMLModelElement, dom::HTMLModelElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;
    HTMLModelElement(const Napi::CallbackInfo &info);

  public:
    static vector<Napi::ClassPropertyDescriptor<HTMLModelElement>> GetClassProperties(Napi::Env env);
    static void Init(Napi::Env env);

  private:
    // Property getters and setters
    Napi::Value SrcGetter(const Napi::CallbackInfo &info);
    void SrcSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value TypeGetter(const Napi::CallbackInfo &info);
    void TypeSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value AutoplayGetter(const Napi::CallbackInfo &info);
    void AutoplaySetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value LoadingGetter(const Napi::CallbackInfo &info);
    void LoadingSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}