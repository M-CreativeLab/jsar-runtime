#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_link_element.hpp>
#include "./html_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLLinkElement;
    using HTMLLinkElementBase = scripting_base::ObjectWrap<HTMLLinkElement, dom::HTMLLinkElement, HTMLElement>;

    class HTMLLinkElement : public HTMLLinkElementBase
    {
      using HTMLLinkElementBase::ObjectWrap;

    public:
      /**
       * The name of the HTMLLinkElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLLinkElement";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<dom::HTMLLinkElement> nativeElement);

    public:
      HTMLLinkElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
