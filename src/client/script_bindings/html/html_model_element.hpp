#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_model_element.hpp>
#include "./html_element.hpp"

namespace endor
{
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
        static std::string Name()
        {
          return "HTMLModelElement";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      public:
        HTMLModelElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
      };
    }
  }
} // namespace endor
