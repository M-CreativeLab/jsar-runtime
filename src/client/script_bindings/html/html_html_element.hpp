#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_html_element.hpp>
#include "./html_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLHtmlElement;
    using HTMLHtmlElementBase = scripting_base::ObjectWrap<HTMLHtmlElement, dom::HTMLHtmlElement, HTMLElement>;

    class HTMLHtmlElement : public HTMLHtmlElementBase
    {
      using HTMLHtmlElementBase::ObjectWrap;

    public:
      /**
       * The name of the HTMLHtmlElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLHtmlElement";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<dom::HTMLHtmlElement> nativeElement);

    public:
      HTMLHtmlElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
