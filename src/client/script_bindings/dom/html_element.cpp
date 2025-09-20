#include "./html_element.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace dom
  {
    // static
    void HTMLElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add property accessors
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "innerText").ToLocalChecked(),
                                    InnerTextGetter,
                                    InnerTextSetter);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "hidden").ToLocalChecked(),
                                    HiddenGetter,
                                    HiddenSetter);

      // Add methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "click").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Click));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "focus").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Focus));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "blur").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Blur));
    }

    // static
    Local<Object> HTMLElement::NewInstance(Isolate *isolate, std::shared_ptr<::dom::HTMLElement> nativeElement)
    {
      EscapableHandleScope scope(isolate);

      if (nativeElement == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<HTMLElement, ::dom::HTMLElement, Element>::NewInstance(isolate, nativeElement).As<Object>());
    }

    // static
    Local<Function> HTMLElement::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<HTMLElement, ::dom::HTMLElement, Element>::Initialize(isolate);
    }

    HTMLElement::HTMLElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<HTMLElement, ::dom::HTMLElement, Element>(isolate, args)
    {
    }

    // Property getters and setters

    // static
    void HTMLElement::InnerTextGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      HTMLElement *element = scripting_base::ObjectWrap<HTMLElement, ::dom::HTMLElement, Element>::Unwrap(info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        info.GetReturnValue().SetEmptyString();
        return;
      }

      string innerText = element->inner()->innerText();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, innerText.c_str()).ToLocalChecked());
    }

    // static
    void HTMLElement::InnerTextSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      HTMLElement *element = scripting_base::ObjectWrap<HTMLElement, ::dom::HTMLElement, Element>::Unwrap(info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value utf8Value(isolate, value);
      element->inner()->setInnerText(string(*utf8Value));
    }

    // static
    void HTMLElement::HiddenGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      HTMLElement *element = scripting_base::ObjectWrap<HTMLElement, ::dom::HTMLElement, Element>::Unwrap(info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
        return;
      }

      bool hidden = element->inner()->hidden();
      info.GetReturnValue().Set(Boolean::New(isolate, hidden));
    }

    // static
    void HTMLElement::HiddenSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      HTMLElement *element = scripting_base::ObjectWrap<HTMLElement, ::dom::HTMLElement, Element>::Unwrap(info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        return;
      }

      bool hidden = value->BooleanValue(isolate);
      element->inner()->setHidden(hidden);
    }

    // Methods

    // static
    void HTMLElement::Click(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      HTMLElement *element = scripting_base::ObjectWrap<HTMLElement, ::dom::HTMLElement, Element>::Unwrap(info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        return;
      }

      element->inner()->click();
    }

    // static
    void HTMLElement::Focus(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      HTMLElement *element = scripting_base::ObjectWrap<HTMLElement, ::dom::HTMLElement, Element>::Unwrap(info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        return;
      }

      element->inner()->focus();
    }

    // static
    void HTMLElement::Blur(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      HTMLElement *element = scripting_base::ObjectWrap<HTMLElement, ::dom::HTMLElement, Element>::Unwrap(info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        return;
      }

      element->inner()->blur();
    }
  }
}