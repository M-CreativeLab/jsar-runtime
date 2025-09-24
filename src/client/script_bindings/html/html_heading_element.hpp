#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/html_heading_element.hpp>
#include "./html_element.hpp"

namespace script_bindings
{
  namespace html
  {
    class HTMLHeadingElement;
    using HTMLHeadingElementBase = scripting_base::ObjectWrap<HTMLHeadingElement, dom::HTMLHeadingElement, HTMLElement>;

    /**
     * HTMLHeadingElement wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps dom::HTMLHeadingElement objects for use in V8 JavaScript execution contexts.
     * It provides the standard HTML heading element interface (h1-h6) including properties like 
     * level for semantic heading level (1-6) for accessibility and document structure.
     */
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

    private:
      // Property getters
      static void LevelGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void AlignGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // Property setters
      static void AlignSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
    };
  }
}