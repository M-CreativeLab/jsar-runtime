#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_span_element.hpp>
#include "./html_element.hpp"

namespace endor
{
  namespace script_bindings
  {
    namespace html_bindings
    {
      class HTMLSpanElement;
      using HTMLSpanElementBase = scripting_base::ObjectWrap<HTMLSpanElement, dom::HTMLSpanElement, HTMLElement>;

      class HTMLSpanElement : public HTMLSpanElementBase
      {
        using HTMLSpanElementBase::ObjectWrap;

      public:
        static std::string Name()
        {
          return "HTMLSpanElement";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      public:
        HTMLSpanElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
      };
    }
  }
} // namespace endor
