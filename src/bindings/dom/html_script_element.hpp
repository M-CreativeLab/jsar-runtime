#pragma once

#include "client/dom/html_script_element.hpp"
#include "./html_element.hpp"

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

  public:
    static Napi::FunctionReference *constructor;
  };
}
