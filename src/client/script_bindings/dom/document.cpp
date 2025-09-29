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
    void Document::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, prototype, "documentElement", &Document::DocumentElementGetter);
      InstanceReadonlyAccessor(isolate, prototype, "head", &Document::HeadGetter);
      InstanceAccessor(isolate, prototype, "body", &Document::BodyGetter, &Document::BodySetter);
      InstanceAccessor(isolate, prototype, "title", &Document::TitleGetter, &Document::TitleSetter);

      // Add methods
      InstanceMethod(isolate, prototype, "createElement", &Document::CreateElement);
      InstanceMethod(isolate, prototype, "createTextNode", &Document::CreateTextNode);
      InstanceMethod(isolate, prototype, "createComment", &Document::CreateComment);
      InstanceMethod(isolate, prototype, "getElementById", &Document::GetElementById);
      InstanceMethod(isolate, prototype, "getElementsByTagName", &Document::GetElementsByTagName);
      InstanceMethod(isolate, prototype, "getElementsByClassName", &Document::GetElementsByClassName);
      InstanceMethod(isolate, prototype, "querySelector", &Document::QuerySelector);
      InstanceMethod(isolate, prototype, "querySelectorAll", &Document::QuerySelectorAll);
    }

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

    void Document::DocumentElementGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto documentElement = handle()->documentElement();
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

    void Document::BodyGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto bodyElement = handle()->body();
      return bodyElement == nullptr
               ? info.GetReturnValue().SetNull()
               : info.GetReturnValue().Set(html_bindings::HTMLBodyElement::GetOrNewInstance(isolate, bodyElement));
    }

    void Document::BodySetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "body", "Setting document.body is not supported")));
      return;
    }

    void Document::HeadGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto headElement = handle()->head();
      return headElement == nullptr
               ? info.GetReturnValue().SetNull()
               : info.GetReturnValue().Set(html_bindings::HTMLHeadElement::GetOrNewInstance(isolate, headElement));
    }

    void Document::TitleGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      // TODO(yorkie): Implement proper title retrieval
      string title = "";
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, title.c_str()).ToLocalChecked());
    }

    void Document::TitleSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      // TODO(yorkie): Implement proper title setting
      String::Utf8Value utf8Value(isolate, value);
      // document->inner()->setTitle(string(*utf8Value));
    }

    // Methods

    void Document::CreateElement(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (info.Length() < 1)
      {
        isolate->ThrowException(
          Exception::TypeError(MakeMethodError(isolate,
                                               "createElement",
                                               "1 argument required, but only 0 present")));
        return;
      }
      if (!info[0]->IsString())
      {
        isolate->ThrowException(
          Exception::TypeError(MakeMethodError(isolate,
                                               "createElement",
                                               "Argument 1 must be a string")));
        return;
      }

      String::Utf8Value tagName(isolate, info[0]->ToString(context).ToLocalChecked());
      string tagNameStr = string(*tagName);

      auto element = handle()->createElement(tagNameStr);
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

    void Document::GetElementById(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "getElementById", "1 argument required, but only 0 present.")));
        return;
      }

      String::Utf8Value id(isolate, info[0]);
      auto element = handle()->getElementById(string(*id));
      if (element != nullptr)
      {
        Local<Object> elementWrapper = Element::GetOrNewInstance(isolate, element);
        info.GetReturnValue().Set(elementWrapper);
      }
      else
      {
        info.GetReturnValue().SetNull();
      }
    }

    void Document::GetElementsByTagName(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "getElementsByTagName", "1 argument required, but only 0 present.")));
        return;
      }

      String::Utf8Value tagName(isolate, info[0]);
      auto elements = handle()->getElementsByTagName(string(*tagName));

      Local<Array> resultArray = Array::New(isolate, elements.size());
      for (size_t i = 0; i < elements.size(); ++i)
      {
        Local<Object> elementWrapper = Element::GetOrNewInstance(isolate, elements[i]);
        resultArray->Set(context, static_cast<uint32_t>(i), elementWrapper).Check();
      }
      info.GetReturnValue().Set(resultArray);
    }

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
