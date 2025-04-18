#pragma once

#include <client/dom/html_link_element.hpp>
#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLLinkElement : public HTMLElementBase<HTMLLinkElement, dom::HTMLLinkElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
