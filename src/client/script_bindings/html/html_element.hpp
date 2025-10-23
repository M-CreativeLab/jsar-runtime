#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/dom/element.hpp>
#include <client/html/html_element.hpp>

namespace endor
{
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
        /**
       * Create the HTML element constructor function, such as Image, Audio, etc.
       * 
       * @param isolate The V8 isolate.
       * @param name The name of the constructor function.
       * @param callback The constructor callback function.
       * @return The created constructor function.
       */
        static v8::Local<v8::Function> CreateElementConstructor(v8::Isolate *isolate,
                                                                const char *name,
                                                                v8::FunctionCallback callback);

      protected:
        void onCreated() override;

      private:
        // Property getters and setters
        void DatasetGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
        void DirGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
        void DirSetter(const v8::FunctionCallbackInfo<v8::Value> &info);
        void StyleGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
        void InnerTextGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
        void InnerTextSetter(const v8::FunctionCallbackInfo<v8::Value> &info);
        void HiddenGetter(const v8::FunctionCallbackInfo<v8::Value> &info);
        void HiddenSetter(const v8::FunctionCallbackInfo<v8::Value> &info);

        // Methods
        void Click(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Focus(const v8::FunctionCallbackInfo<v8::Value> &info);
        void Blur(const v8::FunctionCallbackInfo<v8::Value> &info);
      };
    }
  }
} // namespace endor
