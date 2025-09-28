#include <iostream>
#include <client/script_bindings/html/html_body_element.hpp>
#include <client/script_bindings/html/html_head_element.hpp>

#include "./document.hpp"
#include "./element.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace dom_bindings
  {
    // static
    void Document::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

#define NAME(X) String::NewFromUtf8(isolate, X).ToLocalChecked()
#define METHOD(X) FunctionTemplate::New(isolate, X)

      // Add property accessors
      instanceTemplate->SetAccessor(NAME("documentElement"),
                                    DocumentElementGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(NAME("body"), BodyGetter, BodySetter);
      instanceTemplate->SetAccessor(NAME("head"),
                                    HeadGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);
      instanceTemplate->SetAccessor(NAME("title"), TitleGetter, TitleSetter);

      // Add methods
      instanceTemplate->Set(NAME("createElement"), METHOD(CreateElement));
      instanceTemplate->Set(NAME("createTextNode"), METHOD(CreateTextNode));
      instanceTemplate->Set(NAME("createComment"), METHOD(CreateComment));
      instanceTemplate->Set(NAME("getElementById"), METHOD(GetElementById));
      instanceTemplate->Set(NAME("getElementsByTagName"), METHOD(GetElementsByTagName));
      instanceTemplate->Set(NAME("getElementsByClassName"), METHOD(GetElementsByClassName));
      instanceTemplate->Set(NAME("querySelector"), METHOD(QuerySelector));
      instanceTemplate->Set(NAME("querySelectorAll"), METHOD(QuerySelectorAll));

#undef NAME
#undef METHOD
    }

    // static
    Local<Object> Document::NewInstance(Isolate *isolate, std::shared_ptr<::dom::Document> nativeDocument)
    {
      EscapableHandleScope scope(isolate);

      assert(nativeDocument != nullptr && "Document::NewInstance: nativeDocument is null");
      return scope.Escape(DocumentBase::NewInstance(isolate, nativeDocument));
    }

    Document::Document(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : DocumentBase(isolate, args)
    {
    }

    // Property getters and setters

    // static
    void Document::DocumentElementGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Document *document = Unwrap(isolate, info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      auto documentElement = document->inner()->documentElement();
      if (documentElement == nullptr)
      {
        info.GetReturnValue().SetNull();
      }
      else
      {
        Local<Object> elementWrapper = Element::GetOrNewInstance(isolate, documentElement);
        info.GetReturnValue().Set(elementWrapper);
      }
    }

    // static
    void Document::BodyGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Document *document = Unwrap(isolate, info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      auto bodyElement = document->inner()->body();
      return bodyElement == nullptr
               ? info.GetReturnValue().SetNull()
               : info.GetReturnValue().Set(html_bindings::HTMLBodyElement::GetOrNewInstance(isolate, bodyElement));
    }

    // static
    void Document::BodySetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Document *document = Unwrap(isolate, info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        return;
      }

      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Setting document.body is not supported").ToLocalChecked()));
      return;
    }

    // static
    void Document::HeadGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Document *document = Unwrap(isolate, info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      auto headElement = document->inner()->head();
      return headElement == nullptr
               ? info.GetReturnValue().SetNull()
               : info.GetReturnValue().Set(html_bindings::HTMLHeadElement::GetOrNewInstance(isolate, headElement));
    }

    // static
    void Document::TitleGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Document *document = Unwrap(isolate, info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        info.GetReturnValue().SetEmptyString();
        return;
      }

      // TODO(yorkie): Implement proper title retrieval
      string title = "";
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, title.c_str()).ToLocalChecked());
    }

    // static
    void Document::TitleSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Document *document = Unwrap(isolate, info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        return;
      }

      // TODO(yorkie): Implement proper title setting
      String::Utf8Value utf8Value(isolate, value);
      // document->inner()->setTitle(string(*utf8Value));
    }

    // Methods

    // static
    void Document::CreateElement(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(
          Exception::TypeError(ErrorMessage(isolate,
                                            "createElement",
                                            "1 argument required, but only 0 present")));
        return;
      }

      Document *document = Unwrap(isolate, info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      String::Utf8Value tagName(isolate, info[0]);
      auto element = document->inner()->createElement(string(*tagName));
      if (element != nullptr)
      {
        Local<Object> elementWrapper = Node::NewInstance(isolate, element);
        info.GetReturnValue().Set(elementWrapper);
      }
      else
      {
        info.GetReturnValue().SetNull();
      }
    }

    // static
    void Document::CreateTextNode(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "createTextNode requires 1 argument").ToLocalChecked()));
        return;
      }

      Document *document = Unwrap(isolate, info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      String::Utf8Value data(isolate, info[0]);
      auto textNode = document->inner()->createTextNode(string(*data));

      if (textNode != nullptr)
      {
        // TODO: Create Text node wrapper instead of generic Node
        Local<Object> nodeWrapper = Node::NewInstance(isolate, textNode);
        info.GetReturnValue().Set(nodeWrapper);
      }
      else
      {
        info.GetReturnValue().SetNull();
      }
    }

    // static
    void Document::CreateComment(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "createComment requires 1 argument").ToLocalChecked()));
        return;
      }

      Document *document = Unwrap(isolate, info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      String::Utf8Value data(isolate, info[0]);
      auto commentNode = document->inner()->createComment(string(*data));

      if (commentNode != nullptr)
      {
        // TODO: Create Comment node wrapper instead of generic Node
        Local<Object> nodeWrapper = Node::NewInstance(isolate, commentNode);
        info.GetReturnValue().Set(nodeWrapper);
      }
      else
      {
        info.GetReturnValue().SetNull();
      }
    }

    // static
    void Document::GetElementById(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      Document *document = Unwrap(isolate, info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      String::Utf8Value id(isolate, info[0]);
      auto element = document->inner()->getElementById(string(*id));

      if (element != nullptr)
      {
        Local<Object> elementWrapper = Element::NewInstance(isolate, element);
        info.GetReturnValue().Set(elementWrapper);
      }
      else
      {
        info.GetReturnValue().SetNull();
      }
    }

    // static
    void Document::GetElementsByTagName(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Document *document = Unwrap(isolate, info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        info.GetReturnValue().Set(Array::New(isolate, 0));
        return;
      }

      // TODO: Implement getElementsByTagName - return NodeList
      cout << "getElementsByTagName called" << endl;
      info.GetReturnValue().Set(Array::New(isolate, 0));
    }

    // static
    void Document::GetElementsByClassName(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Document *document = Unwrap(isolate, info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        info.GetReturnValue().Set(Array::New(isolate, 0));
        return;
      }

      // TODO: Implement getElementsByClassName - return NodeList
      cout << "getElementsByClassName called" << endl;
      info.GetReturnValue().Set(Array::New(isolate, 0));
    }

    // static
    void Document::QuerySelector(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Document *document = Unwrap(isolate, info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Implement querySelector
      cout << "querySelector called" << endl;
      info.GetReturnValue().SetNull();
    }

    // static
    void Document::QuerySelectorAll(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Document *document = Unwrap(isolate, info.This());
      if (document == nullptr || document->inner() == nullptr)
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
