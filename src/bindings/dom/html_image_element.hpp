#pragma once

#include "client/dom/html_image_element.hpp"
#include "./html_element.hpp"

namespace dombinding
{
  class HTMLImageElement : public HTMLElementBase<HTMLImageElement, dom::HTMLImageElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static vector<Napi::ClassPropertyDescriptor<HTMLImageElement>> GetClassProperties();
    static void Init(Napi::Env env);

  private:
    Napi::Value CurrentSrcGetter(const Napi::CallbackInfo &info);
    Napi::Value SrcGetter(const Napi::CallbackInfo &info);
    void SrcSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

  public:
    static Napi::FunctionReference *constructor;
  };
}
