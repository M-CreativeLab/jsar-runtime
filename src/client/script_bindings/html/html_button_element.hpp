#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/html_button_element.hpp>
#include "./html_element.hpp"

namespace script_bindings
{
  namespace html
  {
    class HTMLButtonElement;
    using HTMLButtonElementBase = scripting_base::ObjectWrap<HTMLButtonElement, dom::HTMLButtonElement, HTMLElement>;

    /**
     * HTMLButtonElement wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps dom::HTMLButtonElement objects for use in V8 JavaScript execution contexts.
     * It provides the standard HTML button element interface including properties like disabled,
     * type, value, form and methods for form interaction.
     */
    class HTMLButtonElement : public HTMLButtonElementBase
    {
      using HTMLButtonElementBase::ObjectWrap;

    public:
      /**
       * The name of the HTMLButtonElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLButtonElement";
      }

      /**
       * Configure the V8 function template with HTMLButtonElement properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 HTMLButtonElement instance from a native dom::HTMLButtonElement.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::HTMLButtonElement> nativeButtonElement);

      /**
       * Initialize the HTMLButtonElement class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      HTMLButtonElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void DisabledGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void TypeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void ValueGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void FormGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void NameGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // Property setters
      static void DisabledSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void TypeSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void ValueSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void NameSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Methods
      static void Click(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}