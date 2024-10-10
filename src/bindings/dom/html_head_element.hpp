#pragma once

#include "client/dom/html_head_element.hpp"
#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLHeadElement : public HTMLElementBase<HTMLHeadElement, dom::HTMLHeadElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
