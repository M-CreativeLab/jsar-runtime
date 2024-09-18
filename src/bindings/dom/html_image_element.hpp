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

  public:
    static Napi::FunctionReference *constructor;
  };
}
