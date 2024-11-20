#pragma once

#include "client/dom/html_script_element.hpp"
#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLScriptElement : public HTMLElementBase<HTMLScriptElement, dom::HTMLScriptElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static vector<Napi::ClassPropertyDescriptor<HTMLScriptElement>> GetClassProperties();
    static void Init(Napi::Env env);

  private:
    Napi::Value AsyncGetter(const Napi::CallbackInfo &info);
    void AsyncSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value SrcGetter(const Napi::CallbackInfo &info);
    void SrcSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
