#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_heading_element.hpp>
#include "./html_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLHeadingElement;
    using HTMLHeadingElementBase = scripting_base::ObjectWrap<HTMLHeadingElement, dom::HTMLHeadingElement, HTMLElement>;

    class HTMLHeadingElement : public HTMLHeadingElementBase
    {
      using HTMLHeadingElementBase::ObjectWrap;

    public:
      /**
       * The name of the HTMLHeadingElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLHeadingElement";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      HTMLHeadingElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
