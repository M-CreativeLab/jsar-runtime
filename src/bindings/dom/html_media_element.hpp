#pragma once

#include <memory>
#include <napi.h>
#include "client/dom/html_media_element.hpp"
#include "./html_element.hpp"

namespace dombinding
{
  template <typename ObjectType, typename HTMLMediaElementType>
  class HTMLMediaElementBase : public HTMLElementBase<ObjectType, HTMLMediaElementType>
  {
  public:
    using HTMLElementBase<ObjectType, HTMLMediaElementType>::HTMLElementBase;
  };

  class HTMLMediaElement : public HTMLMediaElementBase<HTMLMediaElement, dom::HTMLMediaElement>
  {
  public:
    using HTMLMediaElementBase::HTMLMediaElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static Napi::FunctionReference *constructor;
  };
}
