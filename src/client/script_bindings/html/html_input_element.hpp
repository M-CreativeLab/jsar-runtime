#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/html/html_element.hpp>
#include <client/html/html_input_element.hpp>

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLInputElement;
    using HTMLInputElementBase = scripting_base::ObjectWrap<HTMLInputElement,
                                                            dom::HTMLInputElement,
                                                            HTMLElement>;

    /**
     * HTMLInputElement wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps dom::HTMLInputElement objects for use in V8 JavaScript execution contexts.
     * It provides the HTML input element interface with comprehensive form support.
     */
    class HTMLInputElement : public HTMLInputElementBase
    {
      using HTMLInputElementBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "HTMLInputElement";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      HTMLInputElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Input properties
      static void TypeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void TypeSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void ValueGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void ValueSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void PlaceholderGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void PlaceholderSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void DisabledGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void DisabledSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void RequiredGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void RequiredSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void CheckedGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void CheckedSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void NameGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void NameSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Validation methods
      static void CheckValidity(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ReportValidity(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void SetCustomValidity(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Focus methods
      static void Focus(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Blur(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Select(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}