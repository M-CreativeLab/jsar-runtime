#pragma once

#include "./html_element.hpp"
#include "../cssom/css_style_declaration.hpp"

namespace dombinding
{
  template <typename ObjectType, typename HTMLElementType>
  vector<Napi::ClassPropertyDescriptor<ObjectType>> HTMLElementBase<ObjectType, HTMLElementType>::GetClassProperties(Napi::Env env)
  {
    using T = HTMLElementBase<ObjectType, HTMLElementType>;
    auto props = ElementBase<ObjectType, HTMLElementType>::GetClassProperties(env);
    auto added = vector<Napi::ClassPropertyDescriptor<ObjectType>>(
        {
            T::InstanceAccessor("dataset", &T::DatasetGetter, nullptr),
            T::InstanceAccessor("style", &T::StyleGetter, nullptr),
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  template <typename ObjectType, typename HTMLElementType>
  Napi::Value HTMLElementBase<ObjectType, HTMLElementType>::DatasetGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    auto htmlElement = dynamic_pointer_cast<dom::HTMLElement>(this->node);
    assert(htmlElement != nullptr && "The node is not an HTMLElement.");
    return HTMLElementDataset::NewInstance(env, htmlElement);
  }

  template <typename ObjectType, typename HTMLElementType>
  Napi::Value HTMLElementBase<ObjectType, HTMLElementType>::StyleGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return cssombinding::CSSStyleDeclaration::NewInstance(env, this->node->styleRef());
  }
}
