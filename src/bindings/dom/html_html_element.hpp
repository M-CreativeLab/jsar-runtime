#pragma once

#include <client/html/html_html_element.hpp>
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
    static thread_local Napi::FunctionReference *constructor;
  };
}
