#pragma once

#include <memory>
#include <napi.h>
#include "client/dom/document.hpp"
#include "./node.hpp"

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
          });
      props.insert(props.end(), documentProps.begin(), documentProps.end());
      return props;
    }
    static void Init(Napi::Env env, const std::string &name)
    {
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
      Napi::External<DocumentType> external = Napi::External<DocumentType>::New(env, document.get());
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
      return info.Env().Undefined();
    }
    Napi::Value CreateElementNS(const Napi::CallbackInfo &info)
    {
      return info.Env().Undefined();
    }
  };

  class Document : public DocumentBase<Document, dom::HTMLDocument>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);

  public:
    Document(const Napi::CallbackInfo &info);

  public:
    static Napi::FunctionReference *constructor;
  };

  class XMLDocument : public DocumentBase<XMLDocument, dom::XMLDocument>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);

  public:
    XMLDocument(const Napi::CallbackInfo &info);

  public:
    static Napi::FunctionReference *constructor;
  };
}
