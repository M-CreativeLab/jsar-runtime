#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_script_element.hpp>
#include "./html_element.hpp"

namespace endor
{
  namespace script_bindings
  {
    namespace html_bindings
    {
      class HTMLScriptElement;
      using HTMLScriptElementBase = scripting_base::ObjectWrap<HTMLScriptElement, dom::HTMLScriptElement, HTMLElement>;

      class HTMLScriptElement : public HTMLScriptElementBase
      {
        using HTMLScriptElementBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "HTMLScriptElement";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      private:
        // Property getters/setters
        void AsyncGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void AsyncSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        void DeferGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void DeferSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        void SrcGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void SrcSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        void TypeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void TypeSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        void TextGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void TextSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
        void CharsetGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void CharsetSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

        // Static methods
        static void Supports(const v8::FunctionCallbackInfo<v8::Value> &info);
      };
    }
  }
} // namespace endor
