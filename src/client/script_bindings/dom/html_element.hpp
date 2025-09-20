#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/dom/html_element.hpp>
#include "./element.hpp"

namespace script_bindings
{
  namespace dom
  {
    /**
     * HTMLElement wrapper for V8 objects using scripting_base::ObjectWrap.
     *
     * This class wraps dom::HTMLElement objects for use in V8 JavaScript execution contexts.
     * It provides the standard DOM HTMLElement interface including properties and methods
     * common to all HTML elements.
     */
    class HTMLElement : public scripting_base::ObjectWrap<HTMLElement, ::dom::HTMLElement, Element>
    {
    public:
      /**
       * The name of the HTMLElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLElement";
      }

      /**
       * Configure the V8 function template with HTMLElement properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 HTMLElement instance from a native dom::HTMLElement.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::dom::HTMLElement> nativeElement);

      /**
       * Initialize the HTMLElement class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      HTMLElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters and setters
      static void InnerTextGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void InnerTextSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void HiddenGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void HiddenSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Methods
      static void Click(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Focus(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Blur(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}