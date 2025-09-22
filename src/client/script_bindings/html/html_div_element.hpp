#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_div_element.hpp>
#include "./html_element.hpp"

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

      /**
       * Configure the V8 function template with HTMLDivElement properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 HTMLDivElement instance from a native dom::HTMLDivElement.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::dom::HTMLDivElement> nativeElement);

      /**
       * Initialize the HTMLDivElement class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      HTMLDivElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
