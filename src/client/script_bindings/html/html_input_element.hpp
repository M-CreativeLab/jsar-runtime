#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/html/html_element.hpp>
#include <client/html/html_input_element.hpp>

namespace endor
{
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
        void TypeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void TypeSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        void ValueGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void ValueSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        void PlaceholderGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void PlaceholderSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        void DisabledGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void DisabledSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        void RequiredGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void RequiredSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        void CheckedGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void CheckedSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        void NameGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void NameSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

        // Validation methods
        void CheckValidity(const v8::FunctionCallbackInfo<v8::Value> &info);
        void ReportValidity(const v8::FunctionCallbackInfo<v8::Value> &info);
        void SetCustomValidity(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Focus methods
        void Focus(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Blur(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Select(const v8::FunctionCallbackInfo<v8::Value> &info);
      };
    }
  }
} // namespace endor
