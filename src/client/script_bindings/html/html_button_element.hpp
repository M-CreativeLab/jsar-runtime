#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_button_element.hpp>
#include "./html_element.hpp"

namespace script_bindings
{
  namespace html_bindings
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
      static std::string Name()
      {
        return "HTMLButtonElement";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      HTMLButtonElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      void DisabledGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void TypeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ValueGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void FormGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void NameGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

      // Property setters
      void DisabledSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void TypeSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void ValueSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void NameSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Methods
      void Click(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}