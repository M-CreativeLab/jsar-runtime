#pragma once

#include <memory>
#include <napi.h>
#include <client/dom/element.hpp>
#include "./node.hpp"

namespace dombinding
{
  template <typename ObjectType, typename ElementType>
  class ElementBase : public NodeBase<ObjectType, ElementType>
  {
  public:
    static vector<Napi::ClassPropertyDescriptor<ObjectType>> GetClassProperties(Napi::Env env);

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
    Napi::Value ClassNameGetter(const Napi::CallbackInfo &info);
    void ClassNameSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value IdGetter(const Napi::CallbackInfo &info);
    void IdSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value InnerHTMLGetter(const Napi::CallbackInfo &info);
    void InnerHTMLSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value OuterHTMLGetter(const Napi::CallbackInfo &info);
    void OuterHTMLSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

  private:
    Napi::Value After(const Napi::CallbackInfo &info);
    Napi::Value Animate(const Napi::CallbackInfo &info);
    Napi::Value Append(const Napi::CallbackInfo &info);
    Napi::Value AttachShadow(const Napi::CallbackInfo &info);
    Napi::Value Before(const Napi::CallbackInfo &info);
    Napi::Value CheckVisibility(const Napi::CallbackInfo &info);
    Napi::Value Closest(const Napi::CallbackInfo &info);
    Napi::Value GetAttribute(const Napi::CallbackInfo &info);
    Napi::Value GetAttributeNames(const Napi::CallbackInfo &info);
    Napi::Value GetBoundingClientRect(const Napi::CallbackInfo &info);
    Napi::Value HasAttribute(const Napi::CallbackInfo &info);
    Napi::Value HasAttributes(const Napi::CallbackInfo &info);
  };

  class Element : public ElementBase<Element, dom::Element>
  {
    using ElementBase::ElementBase;

  public:
    static void Init(Napi::Env env);

    /**
     * Create corresponding JS `Element` instance from the element-implementation object.
     *
     * @param env The N-API environment.
     * @param elementNode The element-implementation object.
     * @returns The JS `Element` instance.
     */
    static Napi::Object NewInstance(Napi::Env env, shared_ptr<dom::Node> elementNode);

    /**
     * Create a new JS `Element` instance by the given tag name.
     *
     * @param env The N-API environment.
     * @param namespaceURI The namespace URI of the element.
     * @param tagName The tag name of the element.
     * @param ownerDocument The owner document of the element.
     */
    static Napi::Object NewInstance(Napi::Env env, string namespaceURI, string tagName,
                                    std::shared_ptr<dom::Document> ownerDocument);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
