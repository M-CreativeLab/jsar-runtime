#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_iframe_element.hpp>
#include "./html_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLIframeElement;
    using HTMLIframeElementBase = scripting_base::ObjectWrap<HTMLIframeElement, dom::HTMLIframeElement, HTMLElement>;

    class HTMLIframeElement : public HTMLIframeElementBase
    {
      using HTMLIframeElementBase::ObjectWrap;

    public:
      /**
       * The name of the HTMLIframeElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLIframeElement";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<dom::HTMLIframeElement> nativeElement);

    public:
      HTMLIframeElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
