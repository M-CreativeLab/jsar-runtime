#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_script_element.hpp>
#include "./html_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLScriptElement;
    using HTMLScriptElementBase = scripting_base::ObjectWrap<HTMLScriptElement, dom::HTMLScriptElement, HTMLElement>;

    class HTMLScriptElement : public HTMLScriptElementBase
    {
    public:
      static std::string Name()
      {
        return "HTMLScriptElement";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      HTMLScriptElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
