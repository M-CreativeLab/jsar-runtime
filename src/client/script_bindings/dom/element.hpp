#pragma once

#include <memory>
#include <client/dom/element.hpp>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/dom/node.hpp>

namespace script_bindings
{
  namespace dom_bindings
  {
    class Element;
    using ElementBase = scripting_base::ObjectWrap<Element, ::dom::Element, Node>;

    /**
     * Element wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps dom::Element objects for use in V8 JavaScript execution contexts.
     * It provides the standard DOM Element interface including properties like tagName,
     * className, id and methods like getAttribute, setAttribute, etc.
     */
    class Element : public ElementBase
    {
      using ElementBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "Element";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::dom::Element> nativeElement);

    public:
      Element(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters and setters
      void TagNameGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void IdGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void IdSetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ClassListGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ClassNameGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ClassNameSetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ChildrenGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void FirstElementChildGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void LastElementChildGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void InnerHTMLGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void InnerHTMLSetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void OuterHTMLGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
      void OuterHTMLSetter(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Methods
      void After(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Animate(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Append(const v8::FunctionCallbackInfo<v8::Value> &info);
      void AttachShadow(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Before(const v8::FunctionCallbackInfo<v8::Value> &info);
      void CheckVisibility(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Closest(const v8::FunctionCallbackInfo<v8::Value> &info);
      void GetAttribute(const v8::FunctionCallbackInfo<v8::Value> &info);
      void GetAttributeNames(const v8::FunctionCallbackInfo<v8::Value> &info);
      void GetBoundingClientRect(const v8::FunctionCallbackInfo<v8::Value> &info);
      void HasAttribute(const v8::FunctionCallbackInfo<v8::Value> &info);
      void HasAttributes(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Remove(const v8::FunctionCallbackInfo<v8::Value> &info);
      void RemoveAttribute(const v8::FunctionCallbackInfo<v8::Value> &info);
      void SetAttribute(const v8::FunctionCallbackInfo<v8::Value> &info);
      void SetAttributeNode(const v8::FunctionCallbackInfo<v8::Value> &info);
      void SetAttributeNodeNS(const v8::FunctionCallbackInfo<v8::Value> &info);
      void SetAttributeNS(const v8::FunctionCallbackInfo<v8::Value> &info);
      void GetElementsByTagName(const v8::FunctionCallbackInfo<v8::Value> &info);
      void GetElementsByClassName(const v8::FunctionCallbackInfo<v8::Value> &info);
      void QuerySelector(const v8::FunctionCallbackInfo<v8::Value> &info);
      void QuerySelectorAll(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}
