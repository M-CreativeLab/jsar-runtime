#pragma once

#include <client/dom/html_template_element.hpp>
#include "./html_element-inl.hpp"

namespace dombinding
{
  class HTMLTemplateElement : public HTMLElementBase<HTMLTemplateElement, dom::HTMLTemplateElement>
  {
  public:
    using HTMLElementBase::HTMLElementBase;

  public:
    static void Init(Napi::Env env);
    static std::vector<Napi::ClassPropertyDescriptor<HTMLTemplateElement>> GetClassProperties(Napi::Env env);

  private:
    Napi::Value ContentGetter(const Napi::CallbackInfo &info);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
