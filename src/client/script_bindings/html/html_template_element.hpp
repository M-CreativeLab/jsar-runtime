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
      static std::string Name()
      {
        return "HTMLTemplateElement";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      HTMLTemplateElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
