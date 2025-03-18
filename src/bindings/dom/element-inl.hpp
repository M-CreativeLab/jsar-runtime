#pragma once

#include "./element.hpp"
#include "./node-inl.hpp"

namespace dombinding
{
  template <typename ObjectType, typename ElementType>
  std::vector<Napi::ClassPropertyDescriptor<ObjectType>> ElementBase<ObjectType, ElementType>::GetClassProperties(Napi::Env env)
  {
    using T = ElementBase<ObjectType, ElementType>;
    auto props = NodeBase<ObjectType, ElementType>::GetClassProperties(env);
    auto added = vector<Napi::ClassPropertyDescriptor<ObjectType>>(
        {
            T::InstanceAccessor("className", &T::ClassNameGetter, &T::ClassNameSetter),
            T::InstanceAccessor("id", &T::IdGetter, &T::IdSetter),
            T::InstanceAccessor("innerHTML", &T::InnerHTMLGetter, &T::InnerHTMLSetter),
            T::InstanceAccessor("outerHTML", &T::OuterHTMLGetter, &T::OuterHTMLSetter),
            T::InstanceMethod("after", &T::After),
            T::InstanceMethod("animate", &T::Animate),
            T::InstanceMethod("append", &T::Append),
            T::InstanceMethod("attachShadow", &T::AttachShadow),
            T::InstanceMethod("before", &T::Before),
            T::InstanceMethod("checkVisibility", &T::CheckVisibility),
            T::InstanceMethod("closest", &T::Closest),
            T::InstanceMethod("getAttribute", &T::GetAttribute),
            T::InstanceMethod("getAttributeNames", &T::GetAttributeNames),
            T::InstanceMethod("getBoundingClientRect", &T::GetBoundingClientRect),
            T::InstanceMethod("hasAttribute", &T::HasAttribute),
            T::InstanceMethod("hasAttributes", &T::HasAttributes),
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  /// Properties

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::ClassNameGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::String::New(env, this->node->className());
  }

  template <typename ObjectType, typename ElementType>
  void ElementBase<ObjectType, ElementType>::ClassNameSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    this->node->setClassName(value.As<Napi::String>().Utf8Value());
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::IdGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::String::New(env, this->node->id);
  }

  template <typename ObjectType, typename ElementType>
  void ElementBase<ObjectType, ElementType>::IdSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    this->node->setId(value.As<Napi::String>().Utf8Value());
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::InnerHTMLGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::String::New(env, this->node->getInnerHTML());
  }

  template <typename ObjectType, typename ElementType>
  void ElementBase<ObjectType, ElementType>::InnerHTMLSetter(const Napi::CallbackInfo &info,
                                                             const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    this->node->setInnerHTML(value.As<Napi::String>().Utf8Value());
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::OuterHTMLGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::String::New(env, this->node->getOuterHTML());
  }

  template <typename ObjectType, typename ElementType>
  void ElementBase<ObjectType, ElementType>::OuterHTMLSetter(const Napi::CallbackInfo &info,
                                                             const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    this->node->setOuterHTML(value.As<Napi::String>().Utf8Value());
  }

  /// Methods

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::After(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::Animate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::Append(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::AttachShadow(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::Before(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::CheckVisibility(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::Closest(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::GetAttribute(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "1 argument required, but only 0 present.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto attrName = info[0].ToString().Utf8Value();
    auto attrValue = this->node->getAttribute(attrName);
    return Napi::String::New(env, attrValue);
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::GetAttributeNames(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Array result = Napi::Array::New(env);
    auto attrNames = this->node->getAttributeNames();
    for (size_t i = 0; i < attrNames.size(); i++)
      result[i] = Napi::String::New(env, attrNames[i]);
    return result;
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::GetBoundingClientRect(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // TODO: Implement this method.
    return Napi::Object::New(env);
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::HasAttribute(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "1 argument required, but only 0 present.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto attrName = info[0].ToString().Utf8Value();
    auto hasAttr = this->node->hasAttribute(attrName);
    return Napi::Boolean::New(env, hasAttr);
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::HasAttributes(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, this->node->hasAttributes());
  }
}
