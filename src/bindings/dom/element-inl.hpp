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
            T::InstanceAccessor("firstElementChild", &T::FirstElementChildGetter, nullptr),
            T::InstanceAccessor("lastElementChild", &T::LastElementChildGetter, nullptr),
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
            T::InstanceMethod("remove", &T::Remove),
            T::InstanceMethod("removeAttribute", &T::RemoveAttribute),
            T::InstanceMethod("setAttribute", &T::SetAttribute),
            T::InstanceMethod("setAttributeNode", &T::SetAttributeNode),
            T::InstanceMethod("setAttributeNodeNS", &T::SetAttributeNodeNS),
            T::InstanceMethod("setAttributeNS", &T::SetAttributeNS),
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
  Napi::Value ElementBase<ObjectType, ElementType>::FirstElementChildGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto childElement = this->node->firstElementChild();
    return childElement == nullptr
               ? env.Null()
               : Node::NewInstance(env, childElement);
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::LastElementChildGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto childElement = this->node->lastElementChild();
    return childElement == nullptr
               ? env.Null()
               : Node::NewInstance(env, childElement);
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
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "Failed to call 'after' method: 1 argument required, but only 0 present.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    // After should insert from the end to the beginning.
    for (size_t i = info.Length(); i > 0; i--)
    {
      auto nodeValue = info[i - 1];
      if (nodeValue.IsString())
      {
        std::string text = nodeValue.As<Napi::String>().Utf8Value();
        this->node->after(text);
      }
      else
      {
        auto node = Node::GetImpl(nodeValue);
        if (node != nullptr)
        {
          this->node->after(node);
        }
        else
        {
          Napi::TypeError::New(env, "Failed to call 'after' method: invalid argument").ThrowAsJavaScriptException();
          return env.Undefined();
        }
      }
    }
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
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "Failed to call 'before' method: 1 argument required, but only 0 present.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    for (size_t i = 0; i < info.Length(); i++)
    {
      auto nodeValue = info[i];
      if (nodeValue.IsString())
      {
        std::string text = nodeValue.As<Napi::String>().Utf8Value();
        this->node->before(text);
      }
      else
      {
        auto node = Node::GetImpl(nodeValue);
        if (node != nullptr)
        {
          this->node->before(node);
        }
        else
        {
          Napi::TypeError::New(env, "Failed to call 'before' method: invalid argument").ThrowAsJavaScriptException();
          return env.Undefined();
        }
      }
    }
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

    auto boundingRect = this->node->getBoundingClientRect();
    Napi::Object rectObj = Napi::Object::New(env);
    // TODO(yorkie): implement the DOMRect interface.
    rectObj.Set("x", Napi::Number::New(env, boundingRect.x()));
    rectObj.Set("y", Napi::Number::New(env, boundingRect.y()));
    rectObj.Set("width", Napi::Number::New(env, boundingRect.width()));
    rectObj.Set("height", Napi::Number::New(env, boundingRect.height()));
    return rectObj;
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

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::Remove(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    this->node->remove(); // Just remove the underlying node but not the JS object.
    return env.Undefined();
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::RemoveAttribute(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "1 argument required, but only 0 present.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto attrName = info[0].ToString().Utf8Value();
    this->node->removeAttribute(attrName);
    return env.Undefined();
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::SetAttribute(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "2 arguments required, but only 0 present.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto attrName = info[0].ToString().Utf8Value();
    auto attrValue = info[1].ToString().Utf8Value();
    this->node->setAttribute(attrName, attrValue);
    return env.Undefined();
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::SetAttributeNode(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::TypeError::New(env, "Failed to call 'setAttributeNode' method: not implemented")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::SetAttributeNodeNS(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::TypeError::New(env, "Failed to call 'setAttributeNodeNS' method: not implemented")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }

  template <typename ObjectType, typename ElementType>
  Napi::Value ElementBase<ObjectType, ElementType>::SetAttributeNS(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::TypeError::New(env, "Failed to call 'setAttributeNS' method: not implemented")
        .ThrowAsJavaScriptException();
    return env.Undefined();
  }
}
