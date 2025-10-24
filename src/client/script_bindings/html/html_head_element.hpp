#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_head_element.hpp>
#include "./html_element.hpp"

namespace endor
{
  namespace script_bindings
  {
    namespace html_bindings
    {
      class HTMLHeadElement;
      using HTMLHeadElementBase = scripting_base::ObjectWrap<HTMLHeadElement, dom::HTMLHeadElement, HTMLElement>;

      /**
     * HTMLHeadElement wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps dom::HTMLHeadElement objects for use in V8 JavaScript execution contexts.
     * It provides the standard HTML head element interface including properties like profile
     * for metadata management and document head functionality.
     */
      class HTMLHeadElement : public HTMLHeadElementBase
      {
        using HTMLHeadElementBase::ObjectWrap;

      public:
        /**
       * The name of the HTMLHeadElement class for V8.
       */
        static std::string Name()
        {
          return "HTMLHeadElement";
        }
        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      };
    }
  }
} // namespace endor
