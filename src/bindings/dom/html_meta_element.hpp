#pragma once

#include "client/dom/html_meta_element.hpp"
#include "./html_element.hpp"

namespace dombinding
{
  class HTMLMetaElement : public HTMLElementBase<HTMLMetaElement, dom::HTMLMetaElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static vector<Napi::ClassPropertyDescriptor<HTMLMetaElement>> GetClassProperties();
    static void Init(Napi::Env env);

  private:
    Napi::Value ContentGetter(const Napi::CallbackInfo &info);
    void ContentSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value HttpEquivGetter(const Napi::CallbackInfo &info);
    void HttpEquivSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value MediaGetter(const Napi::CallbackInfo &info);
    void MediaSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value NameGetter(const Napi::CallbackInfo &info);
    void NameSetter(const Napi::CallbackInfo &info, const Napi::Value &value);  

  public:
    static Napi::FunctionReference *constructor;
  };
}
