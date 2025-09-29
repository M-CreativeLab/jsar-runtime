#include <iostream>
#include <client/script_bindings/html/html_element.hpp>
#include "./element.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace dom_bindings
  {
    // static
    void Element::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

      // Add property accessors
      prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "tagName").ToLocalChecked(),
                                     TagNameGetter,
                                     nullptr,
                                     Local<Value>(),
                                     AccessControl::DEFAULT,
                                     PropertyAttribute::ReadOnly);

      prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "id").ToLocalChecked(),
                                     IdGetter,
                                     IdSetter);

      prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "className").ToLocalChecked(),
                                     ClassNameGetter,
                                     ClassNameSetter);

      prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "innerHTML").ToLocalChecked(),
                                     InnerHTMLGetter,
                                     InnerHTMLSetter,
                                     Local<Value>(),
                                     AccessControl::DEFAULT,
                                     PropertyAttribute::DontEnum);

      prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "outerHTML").ToLocalChecked(),
                                     OuterHTMLGetter,
                                     OuterHTMLSetter,
                                     Local<Value>(),
                                     AccessControl::DEFAULT,
                                     PropertyAttribute::DontEnum);

      // Add methods
      prototypeTemplate->Set(String::NewFromUtf8(isolate, "getAttribute").ToLocalChecked(),
                             FunctionTemplate::New(isolate, GetAttribute));

      prototypeTemplate->Set(String::NewFromUtf8(isolate, "setAttribute").ToLocalChecked(),
                             FunctionTemplate::New(isolate, SetAttribute));

      prototypeTemplate->Set(String::NewFromUtf8(isolate, "removeAttribute").ToLocalChecked(),
                             FunctionTemplate::New(isolate, RemoveAttribute));

      prototypeTemplate->Set(String::NewFromUtf8(isolate, "hasAttribute").ToLocalChecked(),
                             FunctionTemplate::New(isolate, HasAttribute));

      prototypeTemplate->Set(String::NewFromUtf8(isolate, "getElementsByTagName").ToLocalChecked(),
                             FunctionTemplate::New(isolate, GetElementsByTagName));

      prototypeTemplate->Set(String::NewFromUtf8(isolate, "getElementsByClassName").ToLocalChecked(),
                             FunctionTemplate::New(isolate, GetElementsByClassName));

      prototypeTemplate->Set(String::NewFromUtf8(isolate, "querySelector").ToLocalChecked(),
                             FunctionTemplate::New(isolate, QuerySelector));

      prototypeTemplate->Set(String::NewFromUtf8(isolate, "querySelectorAll").ToLocalChecked(),
                             FunctionTemplate::New(isolate, QuerySelectorAll));
    }

    // static
    Local<Object> Element::NewInstance(Isolate *isolate, std::shared_ptr<::dom::Element> nativeElement)
    {
      EscapableHandleScope scope(isolate);
      assert(nativeElement != nullptr && "The native element is null.");

      if (nativeElement->isHTMLElement())
      {
        return scope.Escape(html_bindings::HTMLElement::NewInstance(
          isolate, static_pointer_cast<::dom::HTMLElement>(nativeElement)));
      }
      else
      {
        return scope.Escape(ElementBase::NewInstance(isolate, nativeElement).As<Object>());
      }
    }

    Element::Element(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ElementBase(isolate, args)
    {
    }

    // Property getters and setters

    // static
    void Element::TagNameGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        info.GetReturnValue().SetUndefined();
        return;
      }

      string tagName = element->inner()->tagName;
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, tagName.c_str()).ToLocalChecked());
    }

    // static
    void Element::IdGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        info.GetReturnValue().SetEmptyString();
        return;
      }

      string id = element->inner()->getAttribute("id");
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, id.c_str()).ToLocalChecked());
    }

    // static
    void Element::IdSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value utf8Value(isolate, value);
      element->inner()->setAttribute("id", string(*utf8Value));
    }

    // static
    void Element::ClassNameGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        info.GetReturnValue().SetEmptyString();
        return;
      }

      string className = element->inner()->getAttribute("class");
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, className.c_str()).ToLocalChecked());
    }

    // static
    void Element::ClassNameSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value utf8Value(isolate, value);
      element->inner()->setAttribute("class", string(*utf8Value));
    }

    // static
    void Element::InnerHTMLGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        info.GetReturnValue().SetEmptyString();
        return;
      }

      string innerHTML = element->inner()->getInnerHTML();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, innerHTML.c_str()).ToLocalChecked());
    }

    // static
    void Element::InnerHTMLSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value utf8Value(isolate, value);
      element->inner()->setInnerHTML(string(*utf8Value));
    }

    // static
    void Element::OuterHTMLGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        info.GetReturnValue().SetEmptyString();
        return;
      }

      string outerHTML = element->inner()->getOuterHTML();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, outerHTML.c_str()).ToLocalChecked());
    }

    // static
    void Element::OuterHTMLSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value utf8Value(isolate, value);
      element->inner()->setOuterHTML(string(*utf8Value));
    }

    // Methods

    // static
    void Element::GetAttribute(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      String::Utf8Value attrName(isolate, info[0]);
      string value = element->inner()->getAttribute(string(*attrName));

      if (value.empty())
      {
        info.GetReturnValue().SetNull();
      }
      else
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, value.c_str()).ToLocalChecked());
      }
    }

    // static
    void Element::SetAttribute(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "setAttribute requires 2 arguments").ToLocalChecked()));
        return;
      }

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value attrName(isolate, info[0]);
      String::Utf8Value attrValue(isolate, info[1]);

      element->inner()->setAttribute(string(*attrName), string(*attrValue));
    }

    // static
    void Element::RemoveAttribute(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        return;
      }

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value attrName(isolate, info[0]);
      element->inner()->removeAttribute(string(*attrName));
    }

    // static
    void Element::HasAttribute(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
        return;
      }

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
        return;
      }

      String::Utf8Value attrName(isolate, info[0]);
      bool hasAttr = element->inner()->hasAttribute(string(*attrName));
      info.GetReturnValue().Set(Boolean::New(isolate, hasAttr));
    }

    // static
    void Element::GetElementsByTagName(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        info.GetReturnValue().Set(Array::New(isolate, 0));
        return;
      }

      // TODO: Implement getElementsByTagName - return NodeList
      cout << "getElementsByTagName called" << endl;
      info.GetReturnValue().Set(Array::New(isolate, 0));
    }

    // static
    void Element::GetElementsByClassName(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        info.GetReturnValue().Set(Array::New(isolate, 0));
        return;
      }

      // TODO: Implement getElementsByClassName - return NodeList
      cout << "getElementsByClassName called" << endl;
      info.GetReturnValue().Set(Array::New(isolate, 0));
    }

    // static
    void Element::QuerySelector(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Implement querySelector
      cout << "querySelector called" << endl;
      info.GetReturnValue().SetNull();
    }

    // static
    void Element::QuerySelectorAll(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Element *element = Unwrap(isolate, info.This());
      if (element == nullptr || element->inner() == nullptr)
      {
        info.GetReturnValue().Set(Array::New(isolate, 0));
        return;
      }

      // TODO: Implement querySelectorAll - return NodeList
      cout << "querySelectorAll called" << endl;
      info.GetReturnValue().Set(Array::New(isolate, 0));
    }
  }
}
