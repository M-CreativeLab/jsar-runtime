#pragma once

#include <memory>
#include <napi.h>
#include <common/utility.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/document.hpp>
#include <client/dom/element.hpp>
#include <client/dom/node_list.hpp>

#include "./node.hpp"
#include "./node_list.hpp"
#include "./element.hpp"

using namespace std;

namespace dombinding
{
  template <typename ObjectType, typename DocumentType>
  class DocumentBase : public NodeBase<ObjectType, DocumentType>
  {
  public:
    static vector<Napi::ClassPropertyDescriptor<ObjectType>> GetClassProperties(Napi::Env env)
    {
      auto props = NodeBase<ObjectType, DocumentType>::GetClassProperties(env);
      auto documentProps = vector<Napi::ClassPropertyDescriptor<ObjectType>>(
          {
              // Properties
              ObjectType::InstanceAccessor("head", &ObjectType::HeadGetter, nullptr, napi_default_jsproperty),
              ObjectType::InstanceAccessor("body", &ObjectType::BodyGetter, nullptr, napi_default_jsproperty),
              // Methods
              ObjectType::InstanceMethod("adoptNode", &ObjectType::AdoptNode, napi_default_method),
              ObjectType::InstanceMethod("append", &ObjectType::Append, napi_default_method),
              ObjectType::InstanceMethod("close", &ObjectType::Close, napi_default_method),
              ObjectType::InstanceMethod("createAttribute", &ObjectType::CreateAttribute, napi_default_method),
              ObjectType::InstanceMethod("createAttributeNS", &ObjectType::CreateAttributeNS, napi_default_method),
              ObjectType::InstanceMethod("createCDATASection", &ObjectType::CreateCDATASection, napi_default_method),
              ObjectType::InstanceMethod("createComment", &ObjectType::CreateComment, napi_default_method),
              ObjectType::InstanceMethod("createDocumentFragment",
                                         &ObjectType::CreateDocumentFragment, napi_default_method),
              ObjectType::InstanceMethod("createElement", &ObjectType::CreateElement, napi_default_method),
              ObjectType::InstanceMethod("createElementNS", &ObjectType::CreateElementNS, napi_default_method),
              ObjectType::InstanceMethod("createTextNode", &ObjectType::CreateTextNode, napi_default_method),
              ObjectType::InstanceMethod("getElementById",
                                         &ObjectType::GetElementById, napi_default_method),
              ObjectType::InstanceMethod("getElementsByClassName",
                                         &ObjectType::GetElementsByClassName, napi_default_method),
              ObjectType::InstanceMethod("getElementsByName",
                                         &ObjectType::GetElementsByName, napi_default_method),
              ObjectType::InstanceMethod("getElementsByTagName",
                                         &ObjectType::GetElementsByTagName, napi_default_method),
              ObjectType::InstanceMethod("querySelector", &ObjectType::QuerySelector, napi_default_method),
              ObjectType::InstanceMethod("querySelectorAll", &ObjectType::QuerySelectorAll, napi_default_method),
          });
      props.insert(props.end(), documentProps.begin(), documentProps.end());
      return props;
    }
    static void Init(Napi::Env env, const std::string &name)
    {
      Napi::HandleScope scope(env);
      auto props = GetClassProperties(env);
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
    Napi::Value HeadGetter(const Napi::CallbackInfo &info);
    Napi::Value BodyGetter(const Napi::CallbackInfo &info);
    Napi::Value AdoptNode(const Napi::CallbackInfo &info);
    Napi::Value Append(const Napi::CallbackInfo &info);
    Napi::Value Close(const Napi::CallbackInfo &info);
    Napi::Value CreateAttribute(const Napi::CallbackInfo &info);
    Napi::Value CreateAttributeNS(const Napi::CallbackInfo &info);
    Napi::Value CreateCDATASection(const Napi::CallbackInfo &info);
    Napi::Value CreateComment(const Napi::CallbackInfo &info);
    Napi::Value CreateDocumentFragment(const Napi::CallbackInfo &info);
    Napi::Value CreateElement(const Napi::CallbackInfo &info);
    Napi::Value CreateElementNS(const Napi::CallbackInfo &info);
    Napi::Value CreateTextNode(const Napi::CallbackInfo &info);
    Napi::Value GetElementById(const Napi::CallbackInfo &info);
    Napi::Value GetElementsByClassName(const Napi::CallbackInfo &info);
    Napi::Value GetElementsByName(const Napi::CallbackInfo &info);
    Napi::Value GetElementsByTagName(const Napi::CallbackInfo &info);
    Napi::Value QuerySelector(const Napi::CallbackInfo &info);
    Napi::Value QuerySelectorAll(const Napi::CallbackInfo &info);

  private:
    std::unique_ptr<Napi::ObjectReference> headElement = nullptr;
    std::unique_ptr<Napi::ObjectReference> bodyElement = nullptr;
  };

  class Document : public DocumentBase<Document, dom::HTMLDocument>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    static Document *GetCurrent(Napi::Env env);
    static Napi::Value NewInstance(Napi::Env env, shared_ptr<dom::Node> node);

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
