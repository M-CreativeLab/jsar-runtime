#pragma once

#include <client/html/html_div_element.hpp>
#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLDivElement : public HTMLElementBase<HTMLDivElement, dom::HTMLDivElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
