#pragma once

#include <napi.h>

#include "./document.hpp"
#include "./text.hpp"
#include "./node-inl.hpp"
#include "./node_list-inl.hpp"

namespace dombinding
{
  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::HeadGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    if (this->headElement == nullptr)
    {
      auto head = this->node->head();
      if (head != nullptr)
        this->headElement = make_unique<Napi::ObjectReference>(Napi::Persistent(Element::NewInstance(env, head)));
    }
    return this->headElement == nullptr ? env.Null() : this->headElement->Value();
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::BodyGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    if (this->bodyElement == nullptr)
    {
      auto body = this->node->body();
      if (body != nullptr)
        this->bodyElement = make_unique<Napi::ObjectReference>(Napi::Persistent(Element::NewInstance(env, body)));
    }
    return this->bodyElement == nullptr ? env.Null() : this->bodyElement->Value();
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::AdoptNode(const Napi::CallbackInfo &info)
  {
    return info.Env().Undefined();
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::Append(const Napi::CallbackInfo &info)
  {
    return info.Env().Undefined();
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::Close(const Napi::CallbackInfo &info)
  {
    return info.Env().Undefined();
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::CreateAttribute(const Napi::CallbackInfo &info)
  {
    return info.Env().Undefined();
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::CreateAttributeNS(const Napi::CallbackInfo &info)
  {
    return info.Env().Undefined();
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::CreateCDATASection(const Napi::CallbackInfo &info)
  {
    return info.Env().Undefined();
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::CreateComment(const Napi::CallbackInfo &info)
  {
    return info.Env().Undefined();
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::CreateDocumentFragment(const Napi::CallbackInfo &info)
  {
    return info.Env().Undefined();
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::CreateElement(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "Failed to execute 'createElement' on 'Document': 1 argument required, but only 0 present.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto tagName = ToLowerCase(info[0].ToString().Utf8Value());
    if (tagName.empty())
    {
      Napi::TypeError::New(env, "Failed to execute 'createElement' on 'Document': The tag name provided ('') is not a valid name.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    return Element::NewInstance(env, "http://www.w3.org/1999/xhtml", tagName, this->node);
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::CreateElementNS(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      auto msg = "Failed to execute 'createElementNS' on 'Document': 2 arguments required, but only " + to_string(info.Length()) + " present.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto namespaceURI = ToLowerCase(info[0].ToString().Utf8Value());
    auto tagName = ToLowerCase(info[1].ToString().Utf8Value());
    if (tagName.empty())
    {
      Napi::TypeError::New(env, "Failed to execute 'createElementNS' on 'Document': The tag name provided ('') is not a valid name.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    return Element::NewInstance(env, namespaceURI, tagName, this->node);
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::CreateTextNode(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "Failed to execute 'createTextNode' on 'Document': 1 argument required, but only 0 present.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto data = info[0].ToString().Utf8Value();
    auto textNode = this->node->createTextNode(data);
    return NodeBase<Text, dom::Text>::FromImpl(env, textNode);
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::GetElementById(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "Failed to execute 'getElementById' on 'Document': 1 argument required, but only 0 present.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto idStr = info[0].ToString().Utf8Value();
    auto element = this->node->getElementById(idStr);
    if (element == nullptr)
      return env.Null();
    else
      return Element::NewInstance(env, element);
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::GetElementsByClassName(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "Failed to execute 'getElementsByClassName' on 'Document': 1 argument required, but only 0 present.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto className = info[0].ToString().Utf8Value();
    auto elements = this->node->getElementsByClassName(className);
    auto arr = Napi::Array::New(env, elements.size());
    for (size_t i = 0; i < elements.size(); i++)
      arr.Set(i, Element::NewInstance(env, elements[i]));
    return arr;
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::GetElementsByName(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "Failed to execute 'getElementsByName' on 'Document': 1 argument required, but only 0 present.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto name = info[0].ToString().Utf8Value();
    auto elements = this->node->getElementsByName(name);
    auto arr = Napi::Array::New(env, elements.size());
    for (size_t i = 0; i < elements.size(); i++)
      arr.Set(i, Element::NewInstance(env, elements[i]));
    return arr;
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::GetElementsByTagName(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "Failed to execute 'getElementsByTagName' on 'Document': 1 argument required, but only 0 present.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto tagName = info[0].ToString().Utf8Value();
    auto elements = this->node->getElementsByTagName(tagName);
    auto arr = Napi::Array::New(env, elements.size());
    for (size_t i = 0; i < elements.size(); i++)
      arr.Set(i, Element::NewInstance(env, elements[i]));
    return arr;
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::QuerySelector(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(
          env, "Failed to execute 'querySelector' on 'Document': 1 argument required, but only 0 present.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto selectors = info[0].ToString().Utf8Value();
    try
    {
      std::shared_ptr<dom::Element> element = this->node->querySelector(selectors);
      return element == nullptr ? env.Null() : Element::NewInstance(env, element);
    }
    catch (const std::exception &e)
    {
      // TODO: use SyntaxError
      auto msg = "Failed to execute 'querySelector' on 'Document': '" + selectors + "' is not a valid selector.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }

  template <typename ObjectType, typename DocumentType>
  Napi::Value DocumentBase<ObjectType, DocumentType>::QuerySelectorAll(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(
          env, "Failed to execute 'querySelector' on 'Document': 1 argument required, but only 0 present.")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto selectors = info[0].ToString().Utf8Value();
    try
    {
      auto list = this->node->querySelectorAll(selectors);
      return NodeList<dom::Element>::NewInstance(env, list);
    }
    catch (const std::exception &e)
    {
      // TODO: use SyntaxError
      auto msg = "Failed to execute 'querySelector' on 'Document': '" + selectors + "' is not a valid selector.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }
}
