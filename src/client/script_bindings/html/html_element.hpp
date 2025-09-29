#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/dom/element.hpp>
#include <client/html/html_element.hpp>

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLElement;
    using HTMLElementBase = scripting_base::ObjectWrap<HTMLElement,
                                                       dom::HTMLElement,
                                                       dom_bindings::Element>;

    class HTMLElement : public HTMLElementBase
    {
    public:
      /**
       * The name of the HTMLElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLElement";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<::dom::HTMLElement> nativeElement);

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

    // Type alias
    using HTMLSectionElement = HTMLElement;
  }
}
