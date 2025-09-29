#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/html/html_element.hpp>
#include <client/html/html_div_element.hpp>

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLDivElement;
    using HTMLDivElementBase = scripting_base::ObjectWrap<HTMLDivElement,
                                                          dom::HTMLDivElement,
                                                          html_bindings::HTMLElement>;

    class HTMLDivElement : public HTMLDivElementBase
    {
    public:
      /**
       * The name of the HTMLDivElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLDivElement";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      HTMLDivElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
