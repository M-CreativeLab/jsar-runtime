#include <iostream>
#include <client/html/all_html_elements.hpp>
#include <client/script_bindings/cssom/css_style_declaration.hpp>

#include "./html_element_dataset.hpp"
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
#include "./html_section_element.hpp"
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
    Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

    // Add property accessors
    InstanceReadonlyAccessor(isolate, prototype, "dataset", &HTMLElement::DatasetGetter);
    InstanceReadonlyAccessor(isolate, prototype, "style", &HTMLElement::StyleGetter);
    InstanceAccessor(isolate, prototype, "dir", &HTMLElement::DirGetter, &HTMLElement::DirSetter);
    InstanceAccessor(isolate,
                     prototype,
                     "innerText",
                     &HTMLElement::InnerTextGetter,
                     &HTMLElement::InnerTextSetter,
                     PropertyAttribute::DontEnum);
    InstanceAccessor(isolate, prototype, "hidden", &HTMLElement::HiddenGetter, &HTMLElement::HiddenSetter);

    // Add methods
    InstanceMethod(isolate, prototype, "click", &HTMLElement::Click);
    InstanceMethod(isolate, prototype, "focus", &HTMLElement::Focus);
    InstanceMethod(isolate, prototype, "blur", &HTMLElement::Blur);
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

  void HTMLElement::onCreated()
  {
    Element::onCreated();

    // Register common events
    registerEvent("beforetoggle");
    registerEvent("change");
    registerEvent("command");
    registerEvent("drag");
    registerEvent("dragend");
    registerEvent("dragenter");
    registerEvent("dragleave");
    registerEvent("dragover");
    registerEvent("dragstart");
    registerEvent("drop");
    registerEvent("error");
    registerEvent("load");
    registerEvent("toggle");
  }

  // Property getters and setters

  void HTMLElement::DatasetGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    info.GetReturnValue().Set(HTMLElementDataset::NewInstance(isolate, handle()));
  }

  void HTMLElement::DirGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    using Dir = ::dom::HTMLElementDirection;
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    switch (handle()->dir)
    {
    case Dir::LTR:
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, "ltr").ToLocalChecked());
      break;
    case Dir::RTL:
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, "rtl").ToLocalChecked());
      break;
    case Dir::Auto:
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, "auto").ToLocalChecked());
      break;
    default:
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, "").ToLocalChecked());
      break;
    }
  }

  void HTMLElement::DirSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
  {
    using Dir = ::dom::HTMLElementDirection;
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (!value.IsEmpty() && value->IsString())
    {
      String::Utf8Value utf8Value(isolate, value);
      string dirStr(*utf8Value);
      if (dirStr == "ltr")
        handle()->dir = Dir::LTR;
      else if (dirStr == "rtl")
        handle()->dir = Dir::RTL;
      else
        handle()->dir = Dir::Auto;
    }
    else
    {
      handle()->dir = Dir::Auto;
    }
  }

  void HTMLElement::StyleGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto style = handle()->styleRef();
    if (style != nullptr)
    {
      auto styleWrapper = cssom_bindings::CSSStyleDeclaration::GetOrNewInstance(isolate, style);
      info.GetReturnValue().Set(styleWrapper);
    }
    else
    {
      info.GetReturnValue().SetUndefined();
    }
  }

  // static
  void HTMLElement::InnerTextGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    HTMLElement *element = Unwrap(isolate, info.This());
    if (element == nullptr || element->handle() == nullptr)
    {
      info.GetReturnValue().SetEmptyString();
      return;
    }

    string innerText = element->handle()->innerText;
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, innerText.c_str()).ToLocalChecked());
  }

  // static
  void HTMLElement::InnerTextSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    HTMLElement *element = Unwrap(isolate, info.This());
    if (element == nullptr || element->handle() == nullptr)
    {
      return;
    }

    String::Utf8Value utf8Value(isolate, value);
    element->handle()->setInnerText(string(*utf8Value));
  }

  // static
  void HTMLElement::HiddenGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    HTMLElement *element = Unwrap(isolate, info.This());
    if (element == nullptr || element->handle() == nullptr)
    {
      info.GetReturnValue().Set(Boolean::New(isolate, false));
      return;
    }

    bool hidden = element->handle()->hidden;
    info.GetReturnValue().Set(Boolean::New(isolate, hidden));
  }

  // static
  void HTMLElement::HiddenSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    HTMLElement *element = Unwrap(isolate, info.This());
    if (element == nullptr || element->handle() == nullptr)
    {
      return;
    }

    bool hidden = value->BooleanValue(isolate);
    element->handle()->setHidden(hidden);
  }

  // Methods

  // static
  void HTMLElement::Click(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    HTMLElement *element = Unwrap(isolate, info.This());
    if (element == nullptr || element->handle() == nullptr)
    {
      return;
    }

    element->handle()->click();
  }

  // static
  void HTMLElement::Focus(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    HTMLElement *element = Unwrap(isolate, info.This());
    if (element == nullptr || element->handle() == nullptr)
    {
      return;
    }

    element->handle()->focus();
  }

  // static
  void HTMLElement::Blur(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    HTMLElement *element = Unwrap(isolate, info.This());
    if (element == nullptr || element->handle() == nullptr)
    {
      return;
    }

    element->handle()->blur();
  }
}
