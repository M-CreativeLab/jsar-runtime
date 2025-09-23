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
      /**
       * The name of the HTMLBodyElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLBodyElement";
      }

      /**
       * Configure the V8 function template with HTMLBodyElement properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 HTMLBodyElement instance from a native dom::HTMLBodyElement.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::HTMLBodyElement> nativeElement);

      /**
       * Initialize the HTMLBodyElement class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      HTMLBodyElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Event handler properties
      static void OnLoadGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void OnLoadSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void OnUnloadGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void OnUnloadSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void OnBeforeUnloadGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void OnBeforeUnloadSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void OnResizeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void OnResizeSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
    };
  }
}