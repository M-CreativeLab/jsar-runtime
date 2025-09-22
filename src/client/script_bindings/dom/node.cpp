#include <iostream>
#include <client/dom/node.hpp>
#include <client/dom/document.hpp>
#include "./node.hpp"

using namespace std;
using namespace v8;

namespace script_bindings::dom_bindings
{
  // static
  void Node::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);

    // Set up the instance template
    Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

    // Add property accessors
    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "nodeName").ToLocalChecked(),
                                  NodeNameGetter,
                                  nullptr,
                                  Local<Value>(),
                                  AccessControl::DEFAULT,
                                  PropertyAttribute::ReadOnly);

    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "nodeType").ToLocalChecked(),
                                  NodeTypeGetter,
                                  nullptr,
                                  Local<Value>(),
                                  AccessControl::DEFAULT,
                                  PropertyAttribute::ReadOnly);

    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "nodeValue").ToLocalChecked(),
                                  NodeValueGetter,
                                  NodeValueSetter);

    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "parentNode").ToLocalChecked(),
                                  ParentNodeGetter,
                                  nullptr,
                                  Local<Value>(),
                                  AccessControl::DEFAULT,
                                  PropertyAttribute::ReadOnly);

    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "firstChild").ToLocalChecked(),
                                  FirstChildGetter,
                                  nullptr,
                                  Local<Value>(),
                                  AccessControl::DEFAULT,
                                  PropertyAttribute::ReadOnly);

    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "lastChild").ToLocalChecked(),
                                  LastChildGetter,
                                  nullptr,
                                  Local<Value>(),
                                  AccessControl::DEFAULT,
                                  PropertyAttribute::ReadOnly);

    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "nextSibling").ToLocalChecked(),
                                  NextSiblingGetter,
                                  nullptr,
                                  Local<Value>(),
                                  AccessControl::DEFAULT,
                                  PropertyAttribute::ReadOnly);

    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "previousSibling").ToLocalChecked(),
                                  PreviousSiblingGetter,
                                  nullptr,
                                  Local<Value>(),
                                  AccessControl::DEFAULT,
                                  PropertyAttribute::ReadOnly);

    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "ownerDocument").ToLocalChecked(),
                                  OwnerDocumentGetter,
                                  nullptr,
                                  Local<Value>(),
                                  AccessControl::DEFAULT,
                                  PropertyAttribute::ReadOnly);

    instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "textContent").ToLocalChecked(),
                                  TextContentGetter,
                                  TextContentSetter);

    // Add methods
    instanceTemplate->Set(String::NewFromUtf8(isolate, "appendChild").ToLocalChecked(),
                          FunctionTemplate::New(isolate, AppendChild));

    instanceTemplate->Set(String::NewFromUtf8(isolate, "removeChild").ToLocalChecked(),
                          FunctionTemplate::New(isolate, RemoveChild));

    instanceTemplate->Set(String::NewFromUtf8(isolate, "insertBefore").ToLocalChecked(),
                          FunctionTemplate::New(isolate, InsertBefore));

    instanceTemplate->Set(String::NewFromUtf8(isolate, "replaceChild").ToLocalChecked(),
                          FunctionTemplate::New(isolate, ReplaceChild));

    instanceTemplate->Set(String::NewFromUtf8(isolate, "cloneNode").ToLocalChecked(),
                          FunctionTemplate::New(isolate, CloneNode));

    instanceTemplate->Set(String::NewFromUtf8(isolate, "hasChildNodes").ToLocalChecked(),
                          FunctionTemplate::New(isolate, HasChildNodes));

    instanceTemplate->Set(String::NewFromUtf8(isolate, "contains").ToLocalChecked(),
                          FunctionTemplate::New(isolate, Contains));
  }

  // static
  Local<Object> Node::NewInstance(Isolate *isolate, std::shared_ptr<::dom::Node> nativeNode)
  {
    EscapableHandleScope scope(isolate);

    if (nativeNode == nullptr)
    {
      return scope.Escape(Local<Object>());
    }

    // TODO: Create appropriate subclass instances based on node type
    // For now, just create a basic Node wrapper
    return scope.Escape(scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::NewInstance(isolate, nativeNode).As<Object>());
  }

  // static
  Local<Function> Node::Initialize(Isolate *isolate)
  {
    return scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Initialize(isolate);
  }

  Node::Node(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>(isolate, args)
  {
  }

  // Property getters

  // static
  void Node::NodeNameGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      info.GetReturnValue().SetUndefined();
      return;
    }

    string nodeName = node->inner()->nodeName;
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, nodeName.c_str()).ToLocalChecked());
  }

  // static
  void Node::NodeTypeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      info.GetReturnValue().SetUndefined();
      return;
    }

    int nodeType = static_cast<int>(node->inner()->nodeType);
    info.GetReturnValue().Set(Integer::New(isolate, nodeType));
  }

  // static
  void Node::NodeValueGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      info.GetReturnValue().SetNull();
      return;
    }

    optional<string> nodeValue = node->inner()->nodeValue();
    if (!nodeValue.has_value())
    {
      info.GetReturnValue().SetNull();
    }
    else
    {
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, nodeValue.value_or("").c_str()).ToLocalChecked());
    }
  }

  // static
  void Node::NodeValueSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      return;
    }

    if (value->IsNull() || value->IsUndefined())
    {
      node->inner()->setNodeValue("");
    }
    else
    {
      String::Utf8Value utf8Value(isolate, value);
      node->inner()->setNodeValue(string(*utf8Value));
    }
  }

  // static
  void Node::ParentNodeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      info.GetReturnValue().SetNull();
      return;
    }

    auto parentNode = node->inner()->parentNode.lock();
    if (parentNode == nullptr)
    {
      info.GetReturnValue().SetNull();
    }
    else
    {
      Local<Object> parentWrapper = Node::NewInstance(isolate, parentNode);
      info.GetReturnValue().Set(parentWrapper);
    }
  }

  // static
  void Node::FirstChildGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      info.GetReturnValue().SetNull();
      return;
    }

    auto firstChild = node->inner()->firstChild();
    if (firstChild == nullptr)
    {
      info.GetReturnValue().SetNull();
    }
    else
    {
      Local<Object> childWrapper = Node::NewInstance(isolate, firstChild);
      info.GetReturnValue().Set(childWrapper);
    }
  }

  // static
  void Node::LastChildGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      info.GetReturnValue().SetNull();
      return;
    }

    auto lastChild = node->inner()->lastChild();
    if (lastChild == nullptr)
    {
      info.GetReturnValue().SetNull();
    }
    else
    {
      Local<Object> childWrapper = Node::NewInstance(isolate, lastChild);
      info.GetReturnValue().Set(childWrapper);
    }
  }

  // static
  void Node::NextSiblingGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      info.GetReturnValue().SetNull();
      return;
    }

    auto nextSibling = node->inner()->nextSibling();
    if (nextSibling == nullptr)
    {
      info.GetReturnValue().SetNull();
    }
    else
    {
      Local<Object> siblingWrapper = Node::NewInstance(isolate, nextSibling);
      info.GetReturnValue().Set(siblingWrapper);
    }
  }

  // static
  void Node::PreviousSiblingGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      info.GetReturnValue().SetNull();
      return;
    }

    auto previousSibling = node->inner()->previousSibling();
    if (previousSibling == nullptr)
    {
      info.GetReturnValue().SetNull();
    }
    else
    {
      Local<Object> siblingWrapper = Node::NewInstance(isolate, previousSibling);
      info.GetReturnValue().Set(siblingWrapper);
    }
  }

  // static
  void Node::OwnerDocumentGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      info.GetReturnValue().SetNull();
      return;
    }

    shared_ptr<::dom::Document> ownerDocument = node->inner()->getOwnerDocumentReference();
    if (ownerDocument == nullptr)
    {
      info.GetReturnValue().SetNull();
    }
    else
    {
      info.GetReturnValue().Set(Node::NewInstance(isolate, ownerDocument));
    }
  }

  // static
  void Node::TextContentGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      info.GetReturnValue().SetNull();
      return;
    }

    string textContent = node->inner()->textContent();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate, textContent.c_str()).ToLocalChecked());
  }

  // static
  void Node::TextContentSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      return;
    }

    String::Utf8Value utf8Value(isolate, value);
    node->inner()->setTextContent(string(*utf8Value));
  }

  // Methods

  // static
  void Node::AppendChild(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "appendChild requires 1 argument").ToLocalChecked()));
      return;
    }

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      return;
    }

    if (!info[0]->IsObject())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Argument must be a Node").ToLocalChecked()));
      return;
    }

    Node *childNode = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(Local<Object>::Cast(info[0]));
    if (childNode == nullptr || childNode->inner() == nullptr)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Argument must be a Node").ToLocalChecked()));
      return;
    }

    auto result = node->inner()->appendChild(childNode->inner());
    if (result != nullptr)
    {
      Local<Object> resultWrapper = Node::NewInstance(isolate, result);
      info.GetReturnValue().Set(resultWrapper);
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  // static
  void Node::RemoveChild(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "removeChild requires 1 argument").ToLocalChecked()));
      return;
    }

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      return;
    }

    if (!info[0]->IsObject())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Argument must be a Node").ToLocalChecked()));
      return;
    }

    Node *childNode = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(Local<Object>::Cast(info[0]));
    if (childNode == nullptr || childNode->inner() == nullptr)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Argument must be a Node").ToLocalChecked()));
      return;
    }

    auto result = node->inner()->removeChild(childNode->inner());
    if (result != nullptr)
    {
      Local<Object> resultWrapper = Node::NewInstance(isolate, result);
      info.GetReturnValue().Set(resultWrapper);
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  // static
  void Node::InsertBefore(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Illegal arguments").ToLocalChecked()));
      return;
    }

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      return;
    }

    auto newChildNode = Node::Unwrap(info[0].As<Object>())->inner();
    auto refChildNode = Node::Unwrap(info[1].As<Object>())->inner();
    if (newChildNode == nullptr || refChildNode == nullptr)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Arguments must be Nodes").ToLocalChecked()));
      return;
    }

    auto insertedNode = node->inner()->insertBefore(newChildNode, refChildNode);
    if (insertedNode != nullptr)
    {
      Local<Object> resultWrapper = Node::NewInstance(isolate, insertedNode);
      info.GetReturnValue().Set(resultWrapper);
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  // static
  void Node::ReplaceChild(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "replaceChild requires 2 arguments").ToLocalChecked()));
      return;
    }

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      return;
    }

    auto newChildNode = Node::Unwrap(info[0].As<Object>())->inner();
    auto oldChildNode = Node::Unwrap(info[1].As<Object>())->inner();
    if (newChildNode == nullptr || oldChildNode == nullptr)
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Arguments must be Nodes").ToLocalChecked()));
      return;
    }

    auto replacedNode = node->inner()->replaceChild(newChildNode, oldChildNode);
    if (replacedNode != nullptr)
    {
      Local<Object> resultWrapper = Node::NewInstance(isolate, replacedNode);
      info.GetReturnValue().Set(resultWrapper);
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  // static
  void Node::CloneNode(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      return;
    }

    bool deep = false;
    if (info.Length() > 0 && info[0]->IsBoolean())
    {
      deep = info[0]->BooleanValue(isolate);
    }

    auto clonedNode = node->inner()->cloneNode(deep);
    return info.GetReturnValue().Set(Node::NewInstance(isolate, clonedNode));
  }

  // static
  void Node::HasChildNodes(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    Node *node = scripting_base::ObjectWrap<Node, ::dom::Node, EventTarget>::Unwrap(info.This());
    if (node == nullptr || node->inner() == nullptr)
    {
      info.GetReturnValue().Set(Boolean::New(isolate, false));
      return;
    }

    bool hasChildren = node->inner()->hasChildNodes();
    info.GetReturnValue().Set(Boolean::New(isolate, hasChildren));
  }

  // static
  void Node::Contains(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    // TODO(yorkie): implement this method
    isolate->ThrowException(Exception::TypeError(
      String::NewFromUtf8(isolate, "Failed to call 'contains' method: Not implemented").ToLocalChecked()));
    return;
  }
}
