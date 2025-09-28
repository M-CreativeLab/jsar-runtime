#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_model_element.hpp>
#include "./html_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLModelElement;
    using HTMLModelElementBase = scripting_base::ObjectWrap<HTMLModelElement, dom::HTMLModelElement, HTMLElement>;

    class HTMLModelElement : public HTMLModelElementBase
    {
      using HTMLModelElementBase::ObjectWrap;

    public:
      /**
       * The name of the HTMLModelElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLModelElement";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<dom::HTMLModelElement> nativeElement);

    public:
      HTMLModelElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
