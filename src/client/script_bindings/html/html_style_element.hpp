#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_style_element.hpp>
#include "./html_element.hpp"

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
      /**
       * The name of the HTMLStyleElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLStyleElement";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<dom::HTMLStyleElement> nativeElement);

    public:
      HTMLStyleElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
