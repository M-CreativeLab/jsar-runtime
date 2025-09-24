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

      /**
       * Configure the V8 function template with HTMLHeadingElement properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 HTMLHeadingElement instance from a native dom::HTMLHeadingElement.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::HTMLHeadingElement> nativeHeadingElement);

      /**
       * Initialize the HTMLHeadingElement class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      HTMLHeadingElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}