#pragma once

#include <memory>
#include <napi.h>
#include "client/dom/html_element.hpp"
#include "./element.hpp"

namespace dombinding
{
  template <typename ObjectType, typename HTMLElementType>
  class HTMLElementBase : public ElementBase<ObjectType, HTMLElementType>
  {
  public:
    static vector<Napi::ClassPropertyDescriptor<ObjectType>> GetClassProperties();

  public:
    using ElementBase<ObjectType, HTMLElementType>::ElementBase;

  public:
    Napi::Value StyleGetter(const Napi::CallbackInfo &info);
  };

  class HTMLElement : public HTMLElementBase<HTMLElement, dom::HTMLElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static Napi::FunctionReference *constructor;
  };
}
