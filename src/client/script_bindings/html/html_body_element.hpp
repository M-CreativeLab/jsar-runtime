#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/html/html_element.hpp>
#include <client/html/html_body_element.hpp>

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLBodyElement;
    using HTMLBodyElementBase = scripting_base::ObjectWrap<HTMLBodyElement,
                                                           dom::HTMLBodyElement,
                                                           HTMLElement>;

    /**
     * HTMLBodyElement wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps dom::HTMLBodyElement objects for use in V8 JavaScript execution contexts.
     * It provides the HTML body element interface with event handling for page lifecycle.
     */
    class HTMLBodyElement : public HTMLBodyElementBase
    {
      using HTMLBodyElementBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "HTMLBodyElement";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
    };
  }
}
