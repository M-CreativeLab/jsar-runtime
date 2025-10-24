#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_meta_element.hpp>
#include "./html_element.hpp"

namespace endor
{
  namespace script_bindings
  {
    namespace html_bindings
    {
      class HTMLMetaElement;
      using HTMLMetaElementBase = scripting_base::ObjectWrap<HTMLMetaElement, dom::HTMLMetaElement, HTMLElement>;

      class HTMLMetaElement : public HTMLMetaElementBase
      {
        using HTMLMetaElementBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "HTMLMetaElement";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      private:
        void ContentGetter(const v8::PropertyCallbackInfo<v8::Value> &args);
        void ContentSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
        void NameGetter(const v8::PropertyCallbackInfo<v8::Value> &args);
        void NameSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
        void HttpEquivGetter(const v8::PropertyCallbackInfo<v8::Value> &args);
        void HttpEquivSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
        void MediaGetter(const v8::PropertyCallbackInfo<v8::Value> &args);
        void MediaSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &args);
      };
    }
  }
} // namespace endor
