#include <iostream>
#include <client/script_bindings/html/html_body_element.hpp>
#include <client/script_bindings/html/html_head_element.hpp>

#include "./document.hpp"
#include "./document_fragment.hpp"
#include "./element.hpp"
#include "./node_list.hpp"

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
      InstanceMethod(isolate, prototype, "createDocumentFragment", &Document::CreateDocumentFragment);
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

    void Document::CreateDocumentFragment(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto fragment = handle()->createDocumentFragment();
      if (fragment != nullptr)
      {
        Local<Object> fragmentWrapper = DocumentFragment::NewInstance(isolate, fragment);
        info.GetReturnValue().Set(fragmentWrapper);
      }
      else
      {
        info.GetReturnValue().SetNull();
      }
    }

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
          MakeMethodError(isolate, "createTextNode", "1 argument required, but only 0 present.")));
        return;
      }

      String::Utf8Value data(isolate, info[0]);
      auto textNode = handle()->createTextNode(string(*data));
      if (textNode != nullptr)
      {
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
          MakeMethodError(isolate, "createComment", "1 argument required, but only 0 present.")));
        return;
      }

      String::Utf8Value data(isolate, info[0]);
      auto commentNode = handle()->createComment(string(*data));
      if (commentNode != nullptr)
      {
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
      Local<Context> context = isolate->GetCurrentContext();

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "getElementsByClassName", "1 argument required, but only 0 present.")));
        return;
      }

      String::Utf8Value className(isolate, info[0]);
      auto elements = handle()->getElementsByClassName(string(*className));

      Local<Array> resultArray = Array::New(isolate, elements.size());
      for (size_t i = 0; i < elements.size(); ++i)
      {
        Local<Object> elementWrapper = Element::GetOrNewInstance(isolate, elements[i]);
        resultArray->Set(context, static_cast<uint32_t>(i), elementWrapper).Check();
      }
      info.GetReturnValue().Set(resultArray);
    }

    void Document::QuerySelector(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "querySelector", "1 argument required, but only 0 present.")));
        return;
      }

      String::Utf8Value selectors(isolate, info[0]);
      auto element = handle()->querySelector(string(*selectors));
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

    void Document::QuerySelectorAll(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "querySelector", "1 argument required, but only 0 present.")));
        return;
      }

      String::Utf8Value selectors(isolate, info[0]);
      try
      {
        auto elements = make_unique<::dom::NodeList<::dom::Element>>(handle()->querySelectorAll(string(*selectors)));
        Local<Value> nodeList = NodeList::NewInstance(isolate, std::move(elements));
        info.GetReturnValue().Set(nodeList);
        return;
      }
      catch (const exception &e)
      {
        string message = "'" + string(*selectors) + "' is not a valid selector.";
        isolate->ThrowException(Exception::Error(
          MakeMethodError(isolate, "querySelectorAll", message.c_str())));
        return;
      }
    }
  }
}
