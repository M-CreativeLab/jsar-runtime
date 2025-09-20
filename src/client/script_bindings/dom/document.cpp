#include "./document.hpp"
#include "./element.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace dom
  {
    // static
    void Document::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add property accessors
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "documentElement").ToLocalChecked(),
                                    DocumentElementGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "body").ToLocalChecked(),
                                    BodyGetter,
                                    BodySetter);

      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "title").ToLocalChecked(),
                                    TitleGetter,
                                    TitleSetter);

      // Add methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "createElement").ToLocalChecked(),
                            FunctionTemplate::New(isolate, CreateElement));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "createTextNode").ToLocalChecked(),
                            FunctionTemplate::New(isolate, CreateTextNode));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "createComment").ToLocalChecked(),
                            FunctionTemplate::New(isolate, CreateComment));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "getElementById").ToLocalChecked(),
                            FunctionTemplate::New(isolate, GetElementById));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "getElementsByTagName").ToLocalChecked(),
                            FunctionTemplate::New(isolate, GetElementsByTagName));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "getElementsByClassName").ToLocalChecked(),
                            FunctionTemplate::New(isolate, GetElementsByClassName));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "querySelector").ToLocalChecked(),
                            FunctionTemplate::New(isolate, QuerySelector));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "querySelectorAll").ToLocalChecked(),
                            FunctionTemplate::New(isolate, QuerySelectorAll));
    }

    // static
    Local<Object> Document::NewInstance(Isolate *isolate, std::shared_ptr<::dom::Document> nativeDocument)
    {
      EscapableHandleScope scope(isolate);

      if (nativeDocument == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<Document, ::dom::Document>::NewInstance(isolate, nativeDocument).As<Object>());
    }

    // static
    Local<Function> Document::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<Document, ::dom::Document>::Initialize(isolate);
    }

    Document::Document(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<Document, ::dom::Document>(isolate, args)
    {
    }

    // Property getters and setters

    // static
    void Document::DocumentElementGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Document *document = scripting_base::ObjectWrap<Document, ::dom::Document>::Unwrap(info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      auto documentElement = document->inner()->documentElement;
      if (documentElement == nullptr)
      {
        info.GetReturnValue().SetNull();
      }
      else
      {
        Local<Object> elementWrapper = Element::NewInstance(isolate, documentElement);
        info.GetReturnValue().Set(elementWrapper);
      }
    }

    // static
    void Document::BodyGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Document *document = scripting_base::ObjectWrap<Document, ::dom::Document>::Unwrap(info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Implement proper body element lookup
      cout << "body getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    // static
    void Document::BodySetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Document *document = scripting_base::ObjectWrap<Document, ::dom::Document>::Unwrap(info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        return;
      }

      // TODO: Implement proper body element setting
      cout << "body setter called" << endl;
    }

    // static
    void Document::TitleGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Document *document = scripting_base::ObjectWrap<Document, ::dom::Document>::Unwrap(info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        info.GetReturnValue().SetEmptyString();
        return;
      }

      string title = document->inner()->title();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, title.c_str()).ToLocalChecked());
    }

    // static
    void Document::TitleSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      Document *document = scripting_base::ObjectWrap<Document, ::dom::Document>::Unwrap(info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        return;
      }

      String::Utf8Value utf8Value(isolate, value);
      document->inner()->setTitle(string(*utf8Value));
    }

    // Methods

    // static
    void Document::CreateElement(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "createElement requires 1 argument").ToLocalChecked()));
        return;
      }

      Document *document = scripting_base::ObjectWrap<Document, ::dom::Document>::Unwrap(info.This());
      if (document == nullptr || document->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      String::Utf8Value tagName(isolate, info[0]);
      auto element = document->inner()->createElement(string(*tagName));

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

      Document *document = scripting_base::ObjectWrap<Document, ::dom::Document>::Unwrap(info.This());
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

      Document *document = scripting_base::ObjectWrap<Document, ::dom::Document>::Unwrap(info.This());
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

      Document *document = scripting_base::ObjectWrap<Document, ::dom::Document>::Unwrap(info.This());
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

      Document *document = scripting_base::ObjectWrap<Document, ::dom::Document>::Unwrap(info.This());
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

      Document *document = scripting_base::ObjectWrap<Document, ::dom::Document>::Unwrap(info.This());
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

      Document *document = scripting_base::ObjectWrap<Document, ::dom::Document>::Unwrap(info.This());
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

      Document *document = scripting_base::ObjectWrap<Document, ::dom::Document>::Unwrap(info.This());
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