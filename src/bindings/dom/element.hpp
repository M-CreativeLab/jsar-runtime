#pragma once

#include <memory>
#include <napi.h>
#include "client/dom/element.hpp"
#include "./node.hpp"

namespace dombinding
{
  Napi::Object CreateElement(Napi::Env env, shared_ptr<dom::Node> elementNode);
  Napi::Object CreateElement(Napi::Env env, string tagName);

  template <typename ObjectType, typename ElementType>
  class ElementBase : public NodeBase<ObjectType, ElementType>
  {
  public:
    static vector<Napi::ClassPropertyDescriptor<ObjectType>> GetClassProperties()
    {
      using T = ElementBase<ObjectType, ElementType>;
      auto props = NodeBase<ObjectType, ElementType>::GetClassProperties();
      auto added = vector<Napi::ClassPropertyDescriptor<ObjectType>>(
          {
              T::InstanceAccessor("className", &T::ClassNameGetter, &T::ClassNameSetter),
              T::InstanceAccessor("id", &T::IdGetter, &T::IdSetter),
              T::InstanceMethod("after", &T::After),
              T::InstanceMethod("animate", &T::Animate),
              T::InstanceMethod("append", &T::Append),
              T::InstanceMethod("attachShadow", &T::AttachShadow),
              T::InstanceMethod("before", &T::Before),
              T::InstanceMethod("checkVisibility", &T::CheckVisibility),
              T::InstanceMethod("closest", &T::Closest),
              T::InstanceMethod("getAttribute", &T::GetAttribute),
              T::InstanceMethod("getAttributeNames", &T::GetAttributeNames),
              T::InstanceMethod("hasAttribute", &T::HasAttribute),
              T::InstanceMethod("hasAttributes", &T::HasAttributes),
          });
      props.insert(props.end(), added.begin(), added.end());
      return props;
    }

  public:
    ElementBase(const Napi::CallbackInfo &info) : NodeBase<ObjectType, ElementType>(info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      auto jsThis = info.This().As<Napi::Object>();
      jsThis.Set("localName", Napi::String::New(env, this->node->localName));
      jsThis.Set("namespaceURI", Napi::String::New(env, this->node->namespaceURI));
      jsThis.Set("prefix", Napi::String::New(env, this->node->prefix));
      jsThis.Set("tagName", Napi::String::New(env, this->node->tagName));
    }
    virtual ~ElementBase() = default;

  private:
    Napi::Value ClassNameGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return Napi::String::New(env, this->node->className);
    }
    void ClassNameSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
    {
      Napi::Env env = info.Env();
      this->node->setClassName(value.As<Napi::String>().Utf8Value());
    }
    Napi::Value IdGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return Napi::String::New(env, this->node->id);
    }
    void IdSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
    {
      Napi::Env env = info.Env();
      this->node->setId(value.As<Napi::String>().Utf8Value());
    }

  private:
    Napi::Value After(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }
    Napi::Value Animate(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }
    Napi::Value Append(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }
    Napi::Value AttachShadow(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }
    Napi::Value Before(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }
    Napi::Value CheckVisibility(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }
    Napi::Value Closest(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }
    Napi::Value GetAttribute(const Napi::CallbackInfo &info)
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
    Napi::Value GetAttributeNames(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      Napi::Array result = Napi::Array::New(env);
      auto attrNames = this->node->getAttributeNames();
      for (size_t i = 0; i < attrNames.size(); i++)
        result[i] = Napi::String::New(env, attrNames[i]);
      return result;
    }
    Napi::Value HasAttribute(const Napi::CallbackInfo &info)
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
    Napi::Value HasAttributes(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      return Napi::Boolean::New(env, this->node->hasAttributes());
    }
  };

  class Element : public ElementBase<Element, dom::Element>
  {
  public:
    using ElementBase::ElementBase;

  public:
    static void Init(Napi::Env env);

  public:
    static Napi::FunctionReference *constructor;
  };

#define TYPED_ELEMENT_MAP(XX) \
  XX("HTML", HTMLHtmlElement) \
  XX("HEAD", HTMLHeadElement) \
  XX("BODY", HTMLBodyElement) \
  XX("META", HTMLMetaElement) \
  XX("SCRIPT", HTMLScriptElement)
}
