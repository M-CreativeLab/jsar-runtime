#pragma once

#include "./v8_object_wrap.hpp"

namespace scripting_base
{
  // Forward declarations for DOM-like classes
  class DOMNode;
  class DOMElement;
  class HTMLElement;

  /**
   * Example base class representing a DOM Node
   */
  class DOMNode : public ObjectWrap<DOMNode>
  {
  public:
    DOMNode(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
        : ObjectWrap<DOMNode>(isolate, args)
    {
    }

    static std::string Name()
    {
      return "Node";
    }

    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
    {
      // Add Node-specific methods and properties
      tpl->PrototypeTemplate()->Set(
        v8::String::NewFromUtf8(isolate, "nodeType").ToLocalChecked(),
        v8::Number::New(isolate, 1),
        v8::PropertyAttribute::ReadOnly);
    }

    virtual std::string GetNodeName() const
    {
      return "Node";
    }
  };

  /**
   * Example derived class representing a DOM Element, inheriting from Node
   */
  class DOMElement : public ObjectWrap<DOMElement, void, DOMNode>
  {
  public:
    DOMElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
        : ObjectWrap<DOMElement, void, DOMNode>(isolate, args)
    {
    }

    static std::string Name()
    {
      return "Element";
    }

    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
    {
      // Add Element-specific methods and properties
      tpl->PrototypeTemplate()->Set(
        v8::String::NewFromUtf8(isolate, "tagName").ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, "DIV").ToLocalChecked(),
        v8::PropertyAttribute::ReadOnly);
    }

    std::string GetNodeName() const override
    {
      return "Element";
    }

    virtual std::string GetTagName() const
    {
      return "DIV";
    }
  };

  /**
   * Example derived class representing an HTML Element, inheriting from Element
   */
  class HTMLElement : public ObjectWrap<HTMLElement, void, DOMElement>
  {
  public:
    HTMLElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
        : ObjectWrap<HTMLElement, void, DOMElement>(isolate, args)
    {
    }

    static std::string Name()
    {
      return "HTMLElement";
    }

    static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
    {
      // Add HTMLElement-specific methods and properties
      tpl->PrototypeTemplate()->Set(
        v8::String::NewFromUtf8(isolate, "innerHTML").ToLocalChecked(),
        v8::String::NewFromUtf8(isolate, "").ToLocalChecked(),
        v8::PropertyAttribute::None);
    }

    std::string GetNodeName() const override
    {
      return "HTMLElement";
    }

    std::string GetTagName() const override
    {
      return "HTML";
    }

    virtual void SetInnerHTML(const std::string &html)
    {
      // Implementation would go here
    }
  };

  /**
   * Helper function to initialize all example classes in the correct order
   */
  inline void InitializeExampleClasses(v8::Isolate *isolate)
  {
    // Initialize in inheritance order: base classes first
    DOMNode::Initialize(isolate);
    DOMElement::Initialize(isolate);
    HTMLElement::Initialize(isolate);
  }
}