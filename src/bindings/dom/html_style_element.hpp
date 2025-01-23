#pragma once

#include <client/dom/html_style_element.hpp>
#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLStyleElement : public HTMLElementBase<HTMLStyleElement, dom::HTMLStyleElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
