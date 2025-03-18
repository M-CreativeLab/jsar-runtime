#pragma once

#include "client/dom/html_section_element.hpp"
#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLSectionElement : public HTMLElementBase<HTMLSectionElement, dom::HTMLSectionElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
