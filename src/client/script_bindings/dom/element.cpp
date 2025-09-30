#include <iostream>
#include <client/script_bindings/html/html_element.hpp>
#include "./element.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace dom_bindings
  {
    void Element::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, prototype, "tagName", &Element::TagNameGetter);
      InstanceReadonlyAccessor(isolate, prototype, "classList", &Element::ClassListGetter);
      InstanceReadonlyAccessor(isolate, prototype, "children", &Element::ChildrenGetter);
      InstanceReadonlyAccessor(isolate, prototype, "firstElementChild", &Element::FirstElementChildGetter);
      InstanceReadonlyAccessor(isolate, prototype, "lastElementChild", &Element::LastElementChildGetter);
      InstanceAccessor(isolate, prototype, "id", &Element::IdGetter, &Element::IdSetter);
      InstanceAccessor(isolate,
                       prototype,
                       "className",
                       &Element::ClassNameGetter,
                       &Element::ClassNameSetter);
      InstanceAccessor(isolate,
                       prototype,
                       "innerHTML",
                       &Element::InnerHTMLGetter,
                       &Element::InnerHTMLSetter,
                       PropertyAttribute::DontEnum);
      InstanceAccessor(isolate,
                       prototype,
                       "outerHTML",
                       &Element::OuterHTMLGetter,
                       &Element::OuterHTMLSetter,
                       PropertyAttribute::DontEnum);

      // Add methods
      InstanceMethod(isolate, prototype, "append", &Element::Append);
      InstanceMethod(isolate, prototype, "getAttribute", &Element::GetAttribute);
      InstanceMethod(isolate, prototype, "setAttribute", &Element::SetAttribute);
      InstanceMethod(isolate, prototype, "removeAttribute", &Element::RemoveAttribute);
      InstanceMethod(isolate, prototype, "hasAttribute", &Element::HasAttribute);
      InstanceMethod(isolate, prototype, "getElementsByTagName", &Element::GetElementsByTagName);
      InstanceMethod(isolate, prototype, "getElementsByClassName", &Element::GetElementsByClassName);
      InstanceMethod(isolate, prototype, "querySelector", &Element::QuerySelector);
      InstanceMethod(isolate, prototype, "querySelectorAll", &Element::QuerySelectorAll);
    }

    Local<Object> Element::NewInstance(Isolate *isolate, std::shared_ptr<::dom::Element> nativeElement)
    {
      EscapableHandleScope scope(isolate);
      assert(nativeElement != nullptr && "The native element is null.");

      if (nativeElement->isHTMLElement())
      {
        auto htmlElement = static_pointer_cast<::dom::HTMLElement>(nativeElement);
        return scope.Escape(html_bindings::HTMLElement::NewInstance(isolate, htmlElement));
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

    void Element::TagNameGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      string tagName = handle()->tagName;
      info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    tagName.c_str())
                                  .ToLocalChecked());
    }

    void Element::IdGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    handle()->id.c_str())
                                  .ToLocalChecked());
    }

    void Element::IdSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      {
        String::Utf8Value utf8Value(isolate, value);
        handle()->setId(string(*utf8Value));
      }
    }

    void Element::ClassListGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "classList", "Not implemented")));
    }

    void Element::ClassNameGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    handle()->className().c_str())
                                  .ToLocalChecked());
    }

    void Element::ClassNameSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      {
        String::Utf8Value utf8Value(isolate, value);
        handle()->setClassName(string(*utf8Value));
      }
    }

    void Element::ChildrenGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "children", "Not implemented")));
    }

    void Element::FirstElementChildGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto element = handle()->firstElementChild();
      if (element != nullptr)
      {
        auto elementWrapper = Element::NewInstance(isolate, element);
        info.GetReturnValue().Set(elementWrapper);
      }
      else
      {
        info.GetReturnValue().SetNull();
      }
    }

    void Element::LastElementChildGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto element = handle()->lastElementChild();
      if (element != nullptr)
      {
        auto elementWrapper = Element::NewInstance(isolate, element);
        info.GetReturnValue().Set(elementWrapper);
      }
      else
      {
        info.GetReturnValue().SetNull();
      }
    }

    void Element::InnerHTMLGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    handle()->getInnerHTML().c_str())
                                  .ToLocalChecked());
    }

    void Element::InnerHTMLSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      {
        String::Utf8Value utf8Value(isolate, value);
        handle()->setInnerHTML(string(*utf8Value));
      }
    }

    void Element::OuterHTMLGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    handle()->getOuterHTML().c_str())
                                  .ToLocalChecked());
    }

    void Element::OuterHTMLSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      {
        String::Utf8Value utf8Value(isolate, value);
        handle()->setOuterHTML(string(*utf8Value));
      }
    }

    // Methods

    void Element::Append(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "append", "Not implemented")));
    }

    void Element::GetAttribute(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      String::Utf8Value attrName(isolate, info[0]);
      string value = handle()->getAttribute(string(*attrName));
      info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    value.c_str())
                                  .ToLocalChecked());
    }

    void Element::SetAttribute(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "setAttribute", "2 arguments required")));
        return;
      }

      if (!info[0]->IsString() || !info[1]->IsString())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "setAttribute", "Both arguments must be strings")));
        return;
      }

      String::Utf8Value attrName(isolate, info[0]);
      String::Utf8Value attrValue(isolate, info[1]);
      handle()->setAttribute(string(*attrName), string(*attrValue));
      info.GetReturnValue().SetNull();
    }

    void Element::RemoveAttribute(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "removeAttribute", "1 argument required")));
        return;
      }
      if (!info[0]->IsString())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "removeAttribute", "Argument must be a string")));
        return;
      }

      String::Utf8Value attrName(isolate, info[0]);
      handle()->removeAttribute(string(*attrName));
      info.GetReturnValue().SetNull();
    }

    void Element::HasAttribute(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "hasAttribute", "1 argument required")));
        return;
      }
      if (!info[0]->IsString())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "hasAttribute", "Argument must be a string")));
        return;
      }

      String::Utf8Value attrName(isolate, info[0]);
      bool hasAttr = handle()->hasAttribute(string(*attrName));
      info.GetReturnValue().Set(Boolean::New(isolate, hasAttr));
    }

    void Element::GetElementsByTagName(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "getElementsByTagName", "Not implemented")));
    }

    void Element::GetElementsByClassName(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "getElementsByClassName", "Not implemented")));
    }

    void Element::QuerySelector(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "querySelector", "Not implemented")));
    }

    void Element::QuerySelectorAll(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "querySelectorAll", "Not implemented")));
    }
  }
}
