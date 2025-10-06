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
      friend class scripting_base::ObjectWrap<HTMLElement, dom::HTMLElement, dom_bindings::Element>;
      using HTMLElementBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "HTMLElement";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<::dom::HTMLElement> nativeElement);

    protected:
      void onCreated() override;

    private:
      // Property getters and setters
      void DatasetGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void DirGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void DirSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void StyleGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void InnerTextGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void InnerTextSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void HiddenGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void HiddenSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Methods
      void Click(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Focus(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Blur(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}
