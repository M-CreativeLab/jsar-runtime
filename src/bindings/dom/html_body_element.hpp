#pragma once

#include "client/dom/html_body_element.hpp"
#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLBodyElement : public HTMLElementBase<HTMLBodyElement, dom::HTMLBodyElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
