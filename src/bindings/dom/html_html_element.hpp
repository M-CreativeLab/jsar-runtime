#pragma once

#include "client/dom/html_html_element.hpp"
#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLHtmlElement : public HTMLElementBase<HTMLHtmlElement, dom::HTMLHtmlElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static Napi::FunctionReference *constructor;
  };
}
