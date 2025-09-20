#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/element.hpp>
#include "./node.hpp"

namespace script_bindings
{
  namespace dom
  {
    /**
     * Element wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps dom::Element objects for use in V8 JavaScript execution contexts.
     * It provides the standard DOM Element interface including properties like tagName,
     * className, id and methods like getAttribute, setAttribute, etc.
     */
    class Element : public scripting_base::ObjectWrap<Element, ::dom::Element>
    {

    public:
      /**
       * The name of the Element class for V8.
       */
      static std::string Name()
      {
        return "Element";
      }

      /**
       * Configure the V8 function template with Element properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 Element instance from a native dom::Element.
       * This will automatically create the appropriate subclass based on the element type.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::dom::Element> nativeElement);

      /**
       * Initialize the Element class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      Element(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters and setters
      static void TagNameGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void IdGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void IdSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void ClassNameGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void ClassNameSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void InnerHTMLGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void InnerHTMLSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void OuterHTMLGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void OuterHTMLSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Methods
      static void GetAttribute(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void SetAttribute(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void RemoveAttribute(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void HasAttribute(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void GetElementsByTagName(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void GetElementsByClassName(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void QuerySelector(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void QuerySelectorAll(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}