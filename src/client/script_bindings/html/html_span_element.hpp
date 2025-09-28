#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_span_element.hpp>
#include "./html_element.hpp"

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
      /**
       * The name of the HTMLSpanElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLSpanElement";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<dom::HTMLSpanElement> nativeElement);

    public:
      HTMLSpanElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
