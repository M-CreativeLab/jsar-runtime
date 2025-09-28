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
      using HTMLScriptElementBase::ObjectWrap;

    public:
      /**
       * The name of the HTMLScriptElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLScriptElement";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<dom::HTMLScriptElement> nativeElement);

    public:
      HTMLScriptElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
