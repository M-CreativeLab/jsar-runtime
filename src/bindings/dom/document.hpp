#pragma once

#include <memory>
#include <napi.h>
#include "common/utility.hpp"
#include "client/dom/document.hpp"
#include "./node.hpp"
#include "./element.hpp"

using namespace std;

namespace dombinding
{
  template <typename ObjectType, typename DocumentType>
  class DocumentBase : public NodeBase<ObjectType, DocumentType>
  {
  public:
    static vector<Napi::ClassPropertyDescriptor<ObjectType>> GetClassProperties()
    {
      auto props = NodeBase<ObjectType, DocumentType>::GetClassProperties();
      auto documentProps = vector<Napi::ClassPropertyDescriptor<ObjectType>>(
          {
              ObjectType::InstanceMethod("adoptNode", &ObjectType::AdoptNode),
              ObjectType::InstanceMethod("append", &ObjectType::Append),
              ObjectType::InstanceMethod("close", &ObjectType::Close),
              ObjectType::InstanceMethod("createAttribute", &ObjectType::CreateAttribute),
              ObjectType::InstanceMethod("createAttributeNS", &ObjectType::CreateAttributeNS),
              ObjectType::InstanceMethod("createCDATASection", &ObjectType::CreateCDATASection),
              ObjectType::InstanceMethod("createComment", &ObjectType::CreateComment),
              ObjectType::InstanceMethod("createDocumentFragment", &ObjectType::CreateDocumentFragment),
              ObjectType::InstanceMethod("createElement", &ObjectType::CreateElement),
              ObjectType::InstanceMethod("createElementNS", &ObjectType::CreateElementNS),
              ObjectType::InstanceMethod("getElementById", &ObjectType::GetElementById),
          });
      props.insert(props.end(), documentProps.begin(), documentProps.end());
      return props;
    }
    static void Init(Napi::Env env, const std::string &name)
    {
      Napi::HandleScope scope(env);
      auto props = GetClassProperties();
      Napi::Function func = ObjectType::DefineClass(env, name.c_str(), props);
      ObjectType::constructor = new Napi::FunctionReference();
      *ObjectType::constructor = Napi::Persistent(func);
      env.Global().Set(name, func);
    }
    /**
     * Create a new JavaScript instance of the document
     *
     * @param env The env
     * @param document The document
     */
    static Napi::Object NewInstance(Napi::Env env, shared_ptr<DocumentType> document)
    {
      Napi::EscapableHandleScope scope(env);
      NodeContainer<DocumentType> nodeContainer(document);
      auto external = Napi::External<NodeContainer<DocumentType>>::New(env, &nodeContainer);
      Napi::Object obj = ObjectType::constructor->New({external});
      return scope.Escape(obj).ToObject();
    }

  public:
    DocumentBase(const Napi::CallbackInfo &info) : NodeBase<ObjectType, DocumentType>(info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      auto jsThis = info.This().As<Napi::Object>();
      /**
       * Set the `compatMode` property.
       *
       * @see https://developer.mozilla.org/en-US/docs/Web/API/Document/compatMode
       */
      switch (this->node->compatMode)
      {
      case dom::DocumentCompatMode::NO_QUIRKS:
      case dom::DocumentCompatMode::LIMITED_QUIRKS:
        jsThis.Set("compatMode", Napi::String::New(env, "CSS1Compat"));
        break;
      case dom::DocumentCompatMode::QUIRKS:
        jsThis.Set("compatMode", Napi::String::New(env, "BackCompat"));
        break;
      }
      /**
       * Set the `contentType` property.
       *
       * @see https://developer.mozilla.org/en-US/docs/Web/API/Document/compatMode
       */
      jsThis.Set("contentType", Napi::String::New(env, this->node->contentType));
    }
    ~DocumentBase() = default;

  protected:
    Napi::Value AdoptNode(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value Append(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value Close(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value CreateAttribute(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value CreateAttributeNS(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value CreateCDATASection(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value CreateComment(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value CreateDocumentFragment(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
    Napi::Value CreateElement(const Napi::CallbackInfo &info)
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
    Napi::Value CreateElementNS(const Napi::CallbackInfo &info)
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
    Napi::Value GetElementById(const Napi::CallbackInfo &info)
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
  };

  class Document : public DocumentBase<Document, dom::HTMLDocument>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    static Document* GetCurrent(Napi::Env env);

  public:
    Document(const Napi::CallbackInfo &info);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };

  class XMLDocument : public DocumentBase<XMLDocument, dom::XMLDocument>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);

  public:
    XMLDocument(const Napi::CallbackInfo &info);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
