#pragma once

#include <client/html/html_paragraph_element.hpp>
#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLParagraphElement : public HTMLElementBase<HTMLParagraphElement, dom::HTMLParagraphElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
