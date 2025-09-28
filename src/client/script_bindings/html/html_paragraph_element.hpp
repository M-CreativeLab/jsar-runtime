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
      using HTMLParagraphElementBase::ObjectWrap;

    public:
      /**
       * The name of the HTMLParagraphElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLParagraphElement";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<dom::HTMLParagraphElement> nativeElement);

    public:
      HTMLParagraphElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
