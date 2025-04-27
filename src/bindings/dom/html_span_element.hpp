#pragma once

#include <client/html/html_span_element.hpp>
#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLSpanElement : public HTMLElementBase<HTMLSpanElement, dom::HTMLSpanElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
