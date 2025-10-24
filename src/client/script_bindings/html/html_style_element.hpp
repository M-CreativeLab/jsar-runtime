#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_style_element.hpp>
#include "./html_element.hpp"

namespace endor
{
  namespace script_bindings
  {
    namespace html_bindings
    {
      class HTMLStyleElement;
      using HTMLStyleElementBase = scripting_base::ObjectWrap<HTMLStyleElement, dom::HTMLStyleElement, HTMLElement>;

      class HTMLStyleElement : public HTMLStyleElementBase
      {
        using HTMLStyleElementBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "HTMLStyleElement";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      public:
        HTMLStyleElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
      };
    }
  }
} // namespace endor
