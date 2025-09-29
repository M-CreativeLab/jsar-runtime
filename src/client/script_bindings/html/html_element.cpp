#include <iostream>
#include <client/html/all_html_elements.hpp>

#include "./html_audio_element.hpp"
#include "./html_body_element.hpp"
#include "./html_button_element.hpp"
#include "./html_canvas_element.hpp"
#include "./html_div_element.hpp"
#include "./html_head_element.hpp"
#include "./html_heading_element.hpp"
#include "./html_html_element.hpp"
#include "./html_iframe_element.hpp"
#include "./html_image_element.hpp"
#include "./html_input_element.hpp"
#include "./html_link_element.hpp"
#include "./html_media_element.hpp"
#include "./html_meta_element.hpp"
#include "./html_model_element.hpp"
#include "./html_paragraph_element.hpp"
#include "./html_script_element.hpp"
#include "./html_span_element.hpp"
#include "./html_style_element.hpp"
#include "./html_template_element.hpp"
#include "./html_video_element.hpp"
#include "./html_element.hpp"

using namespace std;
using namespace v8;

namespace script_bindings::html_bindings
{
  // static
  void HTMLElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);

    // Set up the instance template
    Local<ObjectTemplate> instanceTemplate = tpl->PrototypeTemplate();

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
    assert(nativeElement != nullptr && "nativeElement should not be null");

#define XX(TAG_NAME_STR, ELEMENT_TYPE)                                    \
  if (nativeElement->is(TAG_NAME_STR))                                    \
  {                                                                       \
    return scope.Escape(html_bindings::ELEMENT_TYPE::NewInstance(         \
      isolate, static_pointer_cast<::dom::ELEMENT_TYPE>(nativeElement))); \
  }

    TYPED_ELEMENT_MAP(XX)
#undef XX

    // Fallback to return the HTMLElement type object.
    return scope.Escape(HTMLElementBase::NewInstance(isolate, nativeElement).As<Object>());
  }

  HTMLElement::HTMLElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLElementBase(isolate, args, false)
  {
  }

  // Property getters and setters

  // static
  void HTMLElement::InnerTextGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    HTMLElement *element = Unwrap(isolate, info.This());
    if (element == nullptr || element->inner() == nullptr)
    {
      info.GetReturnValue().SetEmptyString();
      return;
    }

    string innerText = element->inner()->innerText;
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, innerText.c_str()).ToLocalChecked());
  }

  // static
  void HTMLElement::InnerTextSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    HTMLElement *element = Unwrap(isolate, info.This());
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

    HTMLElement *element = Unwrap(isolate, info.This());
    if (element == nullptr || element->inner() == nullptr)
    {
      info.GetReturnValue().Set(Boolean::New(isolate, false));
      return;
    }

    bool hidden = element->inner()->hidden;
    info.GetReturnValue().Set(Boolean::New(isolate, hidden));
  }

  // static
  void HTMLElement::HiddenSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    HTMLElement *element = Unwrap(isolate, info.This());
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

    HTMLElement *element = Unwrap(isolate, info.This());
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

    HTMLElement *element = Unwrap(isolate, info.This());
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

    HTMLElement *element = Unwrap(isolate, info.This());
    if (element == nullptr || element->inner() == nullptr)
    {
      return;
    }

    element->inner()->blur();
  }
}
