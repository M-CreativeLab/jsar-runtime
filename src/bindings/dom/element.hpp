#pragma once

#include <memory>
#include <napi.h>
#include "client/dom/element.hpp"
#include "./node.hpp"

namespace dombinding
{
  class Element : public NodeBase<Element, dom::Element>
  {
  public:
    using NodeBase::NodeBase;

  public:
    static vector<Napi::ClassPropertyDescriptor<Element>> GetClassProperties();
    static void Init(Napi::Env env);
    Element(const Napi::CallbackInfo &info);
    ~Element();

  private:
    Napi::Value ClassNameGetter(const Napi::CallbackInfo &info);
    void ClassNameSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value IdGetter(const Napi::CallbackInfo &info);
    void IdSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

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
    Napi::Value HasAttribute(const Napi::CallbackInfo &info);
    Napi::Value HasAttributes(const Napi::CallbackInfo &info);

  public:
    static Napi::FunctionReference *constructor;
  };
}
