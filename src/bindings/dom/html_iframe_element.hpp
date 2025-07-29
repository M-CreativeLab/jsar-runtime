#pragma once

#include <client/html/html_iframe_element.hpp>
#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLIframeElement : public HTMLElementBase<HTMLIframeElement, dom::HTMLIframeElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}