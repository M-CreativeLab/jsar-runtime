#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_template_element.hpp>
#include "./html_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLTemplateElement;
    using HTMLTemplateElementBase = scripting_base::ObjectWrap<HTMLTemplateElement,
                                                               dom::HTMLTemplateElement,
                                                               HTMLElement>;

    class HTMLTemplateElement : public HTMLTemplateElementBase
    {
      using HTMLTemplateElementBase::ObjectWrap;

    public:
      /**
       * The name of the HTMLTemplateElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLTemplateElement";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<dom::HTMLTemplateElement> nativeElement);

    public:
      HTMLTemplateElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
