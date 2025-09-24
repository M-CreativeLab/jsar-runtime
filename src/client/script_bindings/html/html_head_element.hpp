#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/html_head_element.hpp>
#include "./html_element.hpp"

namespace script_bindings
{
  namespace html
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

      /**
       * Configure the V8 function template with HTMLHeadElement properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 HTMLHeadElement instance from a native dom::HTMLHeadElement.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::HTMLHeadElement> nativeHeadElement);

      /**
       * Initialize the HTMLHeadElement class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      HTMLHeadElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void ProfileGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // Property setters
      static void ProfileSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
    };
  }
}