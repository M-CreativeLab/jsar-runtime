#pragma once

#include "./html_element.hpp"

namespace dombinding
{
  template <typename ObjectType, typename HTMLElementType>
  vector<Napi::ClassPropertyDescriptor<ObjectType>> HTMLElementBase<ObjectType, HTMLElementType>::GetClassProperties()
  {
    using T = HTMLElementBase<ObjectType, HTMLElementType>;
    auto props = ElementBase<ObjectType, HTMLElementType>::GetClassProperties();
    auto added = vector<Napi::ClassPropertyDescriptor<ObjectType>>(
        {
            T::InstanceAccessor("style", &T::StyleGetter, nullptr),
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  template <typename ObjectType, typename HTMLElementType>
  Napi::Value HTMLElementBase<ObjectType, HTMLElementType>::StyleGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Object::New(env);
  }
}
