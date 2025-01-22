#pragma once

#include "client/dom/html_heading_element.hpp"
#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLHeadingElement : public HTMLElementBase<HTMLHeadingElement, dom::HTMLHeadingElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
