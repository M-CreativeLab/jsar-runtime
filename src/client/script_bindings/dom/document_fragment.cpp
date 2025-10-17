#include <iostream>
#include <client/script_bindings/dom/document.hpp>

#include "./document_fragment.hpp"
#include "./node_list.hpp"
#include "./element.hpp"

using namespace std;
using namespace v8;

namespace script_bindings::dom_bindings
{
  void DocumentFragment::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

    // Add properties
    InstanceReadonlyPropertyAccessor(isolate, prototype, "childElementCount", &DocumentFragment::ChildElementCountGetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "children", &DocumentFragment::ChildrenGetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "firstElementChild", &DocumentFragment::FirstElementChildGetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "lastElementChild", &DocumentFragment::LastElementChildGetter);

    // Add methods
    InstanceMethod(isolate, prototype, "append", &DocumentFragment::Append);
    InstanceMethod(isolate, prototype, "prepend", &DocumentFragment::Prepend);
    InstanceMethod(isolate, prototype, "getElementById", &DocumentFragment::GetElementById);
    InstanceMethod(isolate, prototype, "getElementsByTagName", &DocumentFragment::GetElementsByTagName);
    InstanceMethod(isolate, prototype, "getElementsByClassName", &DocumentFragment::GetElementsByClassName);
    InstanceMethod(isolate, prototype, "querySelector", &DocumentFragment::QuerySelector);
    InstanceMethod(isolate, prototype, "querySelectorAll", &DocumentFragment::QuerySelectorAll);
  }

  DocumentFragment::DocumentFragment(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : DocumentFragmentBase(isolate, args)
  {
  }

  // Method implementations
  void DocumentFragment::Append(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    vector<shared_ptr<dom::Node>> nodes;
    for (int i = 0; i < info.Length(); ++i)
    {
      if (Node::IsInstanceOf(isolate, info[i]))
      {
        auto nodeBinding = Node::Unwrap(isolate, info[i].As<Object>());
        if (nodeBinding != nullptr && nodeBinding->hasData())
        {
          nodes.push_back(nodeBinding->handle());
        }
      }
      else if (info[i]->IsString())
      {
        String::Utf8Value utf8Value(isolate, info[i]);
        auto textNode = handle()->getOwnerDocumentChecked().createTextNode(string(*utf8Value));
        nodes.push_back(textNode);
      }
      else
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "All arguments must be Node or string").ToLocalChecked()));
        return;
      }
    }

    handle()->append(nodes);
    info.GetReturnValue().SetUndefined();
  }

  void DocumentFragment::Prepend(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    vector<shared_ptr<dom::Node>> nodes;
    for (int i = 0; i < info.Length(); ++i)
    {
      if (Node::IsInstanceOf(isolate, info[i]))
      {
        auto nodeBinding = Node::Unwrap(isolate, info[i].As<Object>());
        if (nodeBinding != nullptr && nodeBinding->hasData())
        {
          nodes.push_back(nodeBinding->handle());
        }
      }
      else if (info[i]->IsString())
      {
        String::Utf8Value utf8Value(isolate, info[i]);
        auto textNode = handle()->getOwnerDocumentChecked().createTextNode(string(*utf8Value));
        nodes.push_back(textNode);
      }
      else
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "All arguments must be Node or string").ToLocalChecked()));
        return;
      }
    }

    handle()->prepend(nodes);
    info.GetReturnValue().SetUndefined();
  }

  void DocumentFragment::QuerySelector(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 1 || !info[0]->IsString())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Expected a CSS selector string as the first argument").ToLocalChecked()));
      return;
    }

    String::Utf8Value selector(isolate, info[0]);
    auto result = handle()->querySelector(string(*selector));
    if (result)
    {
      auto nodeBinding = Element::GetOrNewInstance(isolate, result);
      info.GetReturnValue().Set(nodeBinding);
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  void DocumentFragment::QuerySelectorAll(const FunctionCallbackInfo<Value> &info)
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
      Local<Value> nodeList = NodeList::NewInstance(isolate, move(elements));
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

  void DocumentFragment::GetElementById(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(Exception::Error(
      MakeMethodError(isolate, "getElementById", "Not implemented")));
  }

  void DocumentFragment::GetElementsByTagName(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(Exception::Error(
      MakeMethodError(isolate, "getElementsByTagName", "Not implemented")));
  }

  void DocumentFragment::GetElementsByClassName(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(Exception::Error(
      MakeMethodError(isolate, "getElementsByClassName", "Not implemented")));
  }

  // Property getters
  void DocumentFragment::ChildElementCountGetter(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto count = handle()->childElementCount();
    info.GetReturnValue().Set(Integer::New(isolate, count));
  }

  void DocumentFragment::ChildrenGetter(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto children = make_unique<dom::NodeList<dom::Element>>(handle()->children());
    info.GetReturnValue().Set(NodeList::NewInstance(isolate, move(children)));
  }

  void DocumentFragment::FirstElementChildGetter(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto element = handle()->firstElementChild();
    if (element)
    {
      auto elementBinding = Element::GetOrNewInstance(isolate, element);
      info.GetReturnValue().Set(elementBinding);
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  void DocumentFragment::LastElementChildGetter(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto element = handle()->lastElementChild();
    if (element)
    {
      auto elementBinding = Element::GetOrNewInstance(isolate, element);
      info.GetReturnValue().Set(elementBinding);
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }
}