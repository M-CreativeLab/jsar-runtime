#include <iostream>
#include <client/script_bindings/html/html_body_element.hpp>
#include <client/script_bindings/html/html_head_element.hpp>

#include "./document.hpp"
#include "./document_fragment.hpp"
#include "./element.hpp"
#include "./node_list.hpp"
#include "./text.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace dom_bindings
  {
    void Document::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();
      Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, instance, "documentURI", &Document::DocumentURIGetter);
      InstanceReadonlyAccessor(isolate, instance, "documentElement", &Document::DocumentElementGetter);
      InstanceReadonlyAccessor(isolate, instance, "head", &Document::HeadGetter);
      InstanceAccessor(isolate, instance, "body", &Document::BodyGetter, &Document::BodySetter);
      InstanceAccessor(isolate, instance, "title", &Document::TitleGetter, &Document::TitleSetter);

      // Add methods
      {
        InstanceMethod(isolate, prototype, "append", &Document::Append);
        InstanceMethod(isolate, prototype, "adoptNode", &Document::AdoptNode);
        InstanceMethod(isolate, prototype, "importNode", &Document::ImportNode);

        InstanceMethod(isolate, prototype, "createAttribute", &Document::CreateAttribute);
        InstanceMethod(isolate, prototype, "createAttributeNS", &Document::CreateAttributeNS);
        InstanceMethod(isolate, prototype, "createCDATASection", &Document::CreateCDATASection);
        InstanceMethod(isolate, prototype, "createDocumentFragment", &Document::CreateDocumentFragment);
        InstanceMethod(isolate, prototype, "createElement", &Document::CreateElement);
        InstanceMethod(isolate, prototype, "createElementNS", &Document::CreateElement);
        InstanceMethod(isolate, prototype, "createTextNode", &Document::CreateTextNode);
        InstanceMethod(isolate, prototype, "createComment", &Document::CreateComment);

        InstanceMethod(isolate, prototype, "getElementById", &Document::GetElementById);
        InstanceMethod(isolate, prototype, "getElementsByTagName", &Document::GetElementsByTagName);
        InstanceMethod(isolate, prototype, "getElementsByClassName", &Document::GetElementsByClassName);
        InstanceMethod(isolate, prototype, "querySelector", &Document::QuerySelector);
        InstanceMethod(isolate, prototype, "querySelectorAll", &Document::QuerySelectorAll);

        InstanceMethod(isolate, prototype, "close", &Document::Close);
        InstanceMethod(isolate, prototype, "write", &Document::Write);
        InstanceMethod(isolate, prototype, "writeln", &Document::Writeln);
      }
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

    void Document::DocumentURIGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto documentURI = handle()->documentURI();
      info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    documentURI.c_str())
                                  .ToLocalChecked());
    }

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
      if (bodyElement == nullptr)
      {
        info.GetReturnValue().SetNull();
      }
      else
      {
        auto jsValue = html_bindings::HTMLBodyElement::GetOrNewInstance(isolate, bodyElement);
        info.GetReturnValue().Set(jsValue);
      }
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

    void Document::Append(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "append", "Not implemented")));
    }

    void Document::AdoptNode(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "adoptNode", "Not implemented")));
    }

    void Document::ImportNode(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "importNode", "1 argument required, but only 0 present.")));
        return;
      }
      if (Node::IsInstanceOf(isolate, info[0]) == false)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "importNode", "Argument 1 must be a Node.")));
        return;
      }

      Local<Object> nodeObj = info[0]->ToObject(context).ToLocalChecked();
      auto nodeBinding = Node::Unwrap(isolate, nodeObj);
      if (nodeBinding == nullptr || !nodeBinding->hasData())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "importNode", "Invalid node object.")));
        return;
      }

      bool deep = false;
      if (info.Length() > 1)
      {
        if (!info[1]->IsBoolean())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "importNode", "Argument 2 must be a boolean.")));
          return;
        }
        deep = info[1]->BooleanValue(isolate);
      }

      auto importedNode = handle()->importNode(nodeBinding->handle(), deep);
      if (importedNode != nullptr)
      {
        info.GetReturnValue().Set(Node::NewInstance(isolate, importedNode));
      }
      else
      {
        info.GetReturnValue().SetNull();
      }
    }

    void Document::CreateAttribute(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "createAttribute", "Not implemented")));
    }

    void Document::CreateAttributeNS(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "createAttributeNS", "Not implemented")));
    }

    void Document::CreateCDATASection(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "createCDATASection", "Not implemented")));
    }

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

    void Document::CreateElementNS(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (info.Length() < 2)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "createElementNS", "2 arguments required, but fewer were provided.")));
        return;
      }
      if (!info[0]->IsString() || !info[1]->IsString())
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "createElementNS", "Both arguments must be strings.")));
        return;
      }

      String::Utf8Value namespaceURI(isolate, info[0]->ToString(context).ToLocalChecked());
      String::Utf8Value qualifiedName(isolate, info[1]->ToString(context).ToLocalChecked());

      auto element = handle()->createElementNS(string(*namespaceURI), string(*qualifiedName));
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

    void Document::Close(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      isolate->ThrowException(Exception::Error(
        MakeMethodError(isolate, "close", "Not implemented")));
    }

    void Document::Write(const v8::FunctionCallbackInfo<v8::Value> &info, bool newLine)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "write", "At least 1 argument required, but none provided.")));
        return;
      }

      string content;
      for (int i = 0; i < info.Length(); ++i)
      {
        auto arg = info[i];
        if (arg->IsString())
        {
          String::Utf8Value data(isolate, arg);
          content += string(*data);
          continue;
        }
        else if (Text::IsInstanceOf(isolate, arg))
        {
          auto textBinding = Text::Unwrap(isolate, arg.As<Object>());
          if (textBinding != nullptr && textBinding->hasData())
          {
            content += textBinding->handle()->data();
            content += "\n";
            continue;
          }
        }

        // For other object types, throw an error for now
        isolate->ThrowException(Exception::TypeError(
          MakeMethodError(isolate, "write", "Only string or Text node arguments are supported.")));
        return;
      }

      newLine ? handle()->writeln(content)
              : handle()->write(content);
      info.GetReturnValue().SetUndefined();
    }

    void Document::Write(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      Write(info, false);
    }

    void Document::Writeln(const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      Write(info, true);
    }
  }
}
