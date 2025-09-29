#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_paragraph_element.hpp>
#include "./html_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLParagraphElement;
    using HTMLParagraphElementBase = scripting_base::ObjectWrap<HTMLParagraphElement, dom::HTMLParagraphElement, HTMLElement>;

    class HTMLParagraphElement : public HTMLParagraphElementBase
    {
    public:
      static std::string Name()
      {
        return "HTMLParagraphElement";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      HTMLParagraphElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
