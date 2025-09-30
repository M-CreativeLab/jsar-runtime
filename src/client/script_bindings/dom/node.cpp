#include <iostream>
#include <client/dom/node.hpp>
#include <client/dom/document.hpp>
#include <client/script_bindings/dom/node.hpp>
#include <client/script_bindings/dom/node_list.hpp>
#include <client/script_bindings/dom/element.hpp>
#include <client/script_bindings/dom/character_data.hpp>
#include <client/script_bindings/dom/document.hpp>
#include <client/script_bindings/dom/document_fragment.hpp>

using namespace std;
using namespace v8;

namespace script_bindings::dom_bindings
{
  void Node::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

#define NODE_TYPE_MAP(XX)         \
  XX(ATTRIBUTE_NODE)              \
  XX(CDATA_SECTION_NODE)          \
  XX(COMMENT_NODE)                \
  XX(DOCUMENT_FRAGMENT_NODE)      \
  XX(DOCUMENT_NODE)               \
  XX(DOCUMENT_TYPE_NODE)          \
  XX(ELEMENT_NODE)                \
  XX(ENTITY_NODE)                 \
  XX(ENTITY_REFERENCE_NODE)       \
  XX(NOTATION_NODE)               \
  XX(PROCESSING_INSTRUCTION_NODE) \
  XX(TEXT_NODE)
#define XX(TYPE) \
  IntegerConstant(isolate, tpl, #TYPE, ::dom::NodeType::TYPE);

    NODE_TYPE_MAP(XX)
#undef XX
#undef NODE_TYPE_MAP

    // Add property accessors
    InstanceReadonlyAccessor(isolate, prototype, "baseURI", &Node::BaseURIGetter);
    InstanceReadonlyAccessor(isolate, prototype, "nodeName", &Node::NodeNameGetter);
    InstanceReadonlyAccessor(isolate, prototype, "nodeType", &Node::NodeTypeGetter);
    InstanceReadonlyAccessor(isolate, prototype, "isConnected", &Node::IsConnectedGetter);
    InstanceReadonlyAccessor(isolate, prototype, "parentNode", &Node::ParentNodeGetter);
    InstanceReadonlyAccessor(isolate, prototype, "parentElement", &Node::ParentElementGetter);
    InstanceReadonlyAccessor(isolate, prototype, "childNodes", &Node::ChildNodesGetter);
    InstanceReadonlyAccessor(isolate, prototype, "firstChild", &Node::FirstChildGetter);
    InstanceReadonlyAccessor(isolate, prototype, "lastChild", &Node::LastChildGetter);
    InstanceReadonlyAccessor(isolate, prototype, "nextSibling", &Node::NextSiblingGetter);
    InstanceReadonlyAccessor(isolate, prototype, "previousSibling", &Node::PreviousSiblingGetter);
    InstanceReadonlyAccessor(isolate, prototype, "ownerDocument", &Node::OwnerDocumentGetter);
    InstanceAccessor(isolate, prototype, "nodeValue", &Node::NodeValueGetter, &Node::NodeValueSetter);
    InstanceAccessor(isolate, prototype, "textContent", &Node::TextContentGetter, &Node::TextContentSetter);

    // Add methods
    InstanceMethod(isolate, prototype, "appendChild", &Node::AppendChild);
    InstanceMethod(isolate, prototype, "removeChild", &Node::RemoveChild);
    InstanceMethod(isolate, prototype, "insertBefore", &Node::InsertBefore);
    InstanceMethod(isolate, prototype, "replaceChild", &Node::ReplaceChild);
    InstanceMethod(isolate, prototype, "cloneNode", &Node::CloneNode);
    InstanceMethod(isolate, prototype, "hasChildNodes", &Node::HasChildNodes);
    InstanceMethod(isolate, prototype, "contains", &Node::Contains);
    InstanceMethod(isolate, prototype, "isDefaultNamespace", &Node::IsDefaultNamespace);
    InstanceMethod(isolate, prototype, "isEqualNode", &Node::IsEqualNode);
    InstanceMethod(isolate, prototype, "isSameNode", &Node::IsSameNode);
    InstanceMethod(isolate, prototype, "normalize", &Node::Normalize);
  }

  Local<Object> Node::NewInstance(Isolate *isolate, std::shared_ptr<::dom::Node> nativeNode)
  {
    EscapableHandleScope scope(isolate);
    assert(nativeNode != nullptr && "The native node is null.");

    if (nativeNode->isDocument())
    {
      return scope.Escape(Document::NewInstance(
        isolate, static_pointer_cast<::dom::Document>(nativeNode)));
    }
    else if (nativeNode->isDocumentFragment())
    {
      return scope.Escape(DocumentFragment::NewInstance(
        isolate, static_pointer_cast<::dom::DocumentFragment>(nativeNode)));
    }
    else if (nativeNode->isElement())
    {
      return scope.Escape(Element::NewInstance(
        isolate, static_pointer_cast<::dom::Element>(nativeNode)));
    }
    else if (nativeNode->isCharacterData())
    {
      return scope.Escape(CharacterData::NewInstance(
        isolate, static_pointer_cast<::dom::CharacterData>(nativeNode)));
    }
    else
    {
      return scope.Escape(NodeBase::NewInstance(isolate, nativeNode));
    }
  }

  Node::Node(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : NodeBase(isolate, args, true)
  {
  }

  // Property getters

  void Node::BaseURIGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                  handle()->baseURI.c_str())
                                .ToLocalChecked());
  }

  void Node::NodeNameGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    string nodeName = handle()->nodeName;
    info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                  nodeName.c_str())
                                .ToLocalChecked());
  }

  void Node::NodeTypeGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    int nodeType = static_cast<int>(handle()->nodeType);
    info.GetReturnValue().Set(Integer::New(isolate, nodeType));
  }

  void Node::NodeValueGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    optional<string> nodeValue = handle()->nodeValue();
    if (!nodeValue.has_value())
      info.GetReturnValue().SetNull();
    else
      info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    nodeValue.value_or("").c_str())
                                  .ToLocalChecked());
  }

  void Node::NodeValueSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (value->IsNull() || value->IsUndefined())
    {
      handle()->setNodeValue("");
    }
    else
    {
      String::Utf8Value utf8Value(isolate, value);
      handle()->setNodeValue(string(*utf8Value));
    }
  }

  void Node::IsConnectedGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    info.GetReturnValue().Set(Boolean::New(isolate, handle()->connected));
  }

  void Node::ParentNodeGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto parentNode = handle()->parentNode.lock();
    if (parentNode == nullptr)
    {
      info.GetReturnValue().SetNull();
    }
    else
    {
      Local<Object> parentWrapper = Node::GetOrNewInstance(isolate, parentNode);
      info.GetReturnValue().Set(parentWrapper);
    }
  }

  void Node::ParentElementGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto parentElement = handle()->getParentElement();
    if (parentElement == nullptr)
    {
      info.GetReturnValue().SetNull();
    }
    else
    {
      Local<Object> parentWrapper = Element::GetOrNewInstance(isolate, parentElement);
      info.GetReturnValue().Set(parentWrapper);
    }
  }

  void Node::ChildNodesGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    auto childNodes = handle()->childNodes;
    Local<Array> resultArray = Array::New(isolate, childNodes.size());
    for (size_t i = 0; i < childNodes.size(); ++i)
    {
      Local<Object> nodeWrapper = Node::GetOrNewInstance(isolate, childNodes[i]);
      resultArray->Set(context, static_cast<uint32_t>(i), nodeWrapper).Check();
    }
    info.GetReturnValue().Set(resultArray);
  }

  void Node::FirstChildGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto firstChild = handle()->firstChild();
    if (firstChild == nullptr)
    {
      info.GetReturnValue().SetNull();
    }
    else
    {
      Local<Object> childWrapper = Node::GetOrNewInstance(isolate, firstChild);
      info.GetReturnValue().Set(childWrapper);
    }
  }

  void Node::LastChildGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto lastChild = handle()->lastChild();
    if (lastChild == nullptr)
    {
      info.GetReturnValue().SetNull();
    }
    else
    {
      Local<Object> childWrapper = Node::GetOrNewInstance(isolate, lastChild);
      info.GetReturnValue().Set(childWrapper);
    }
  }

  void Node::NextSiblingGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto nextSibling = handle()->nextSibling();
    if (nextSibling == nullptr)
    {
      info.GetReturnValue().SetNull();
    }
    else
    {
      Local<Object> siblingWrapper = Node::GetOrNewInstance(isolate, nextSibling);
      info.GetReturnValue().Set(siblingWrapper);
    }
  }

  void Node::PreviousSiblingGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    auto previousSibling = handle()->previousSibling();
    if (previousSibling == nullptr)
    {
      info.GetReturnValue().SetNull();
    }
    else
    {
      Local<Object> siblingWrapper = Node::GetOrNewInstance(isolate, previousSibling);
      info.GetReturnValue().Set(siblingWrapper);
    }
  }

  void Node::OwnerDocumentGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    shared_ptr<::dom::Document> ownerDocument = handle()->getOwnerDocumentReference();
    if (ownerDocument == nullptr)
    {
      info.GetReturnValue().SetNull();
    }
    else
    {
      info.GetReturnValue().Set(Node::GetOrNewInstance(isolate, ownerDocument));
    }
  }

  void Node::TextContentGetter(const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    string textContent = handle()->textContent();
    info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                  textContent.c_str())
                                .ToLocalChecked());
  }

  void Node::TextContentSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    String::Utf8Value utf8Value(isolate, value);
    handle()->setTextContent(string(*utf8Value));
  }

  // Methods

  void Node::AppendChild(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "appendChild", "appendChild requires 1 argument")));
      return;
    }

    if (!info[0]->IsObject())
    {
      isolate->ThrowException(Exception::TypeError(
        String::NewFromUtf8(isolate, "Argument must be a Node").ToLocalChecked()));
      return;
    }

    Node *childNode = Unwrap(isolate, info[0]->ToObject(context).ToLocalChecked());
    if (childNode == nullptr || childNode->handle() == nullptr)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "appendChild", "Argument must be a Node")));
      return;
    }

    auto result = handle()->appendChild(childNode->handle());
    if (result != nullptr)
    {
      Local<Object> resultWrapper = Node::GetOrNewInstance(isolate, result);
      info.GetReturnValue().Set(resultWrapper);
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  void Node::RemoveChild(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "removeChild", "removeChild requires 1 argument")));
      return;
    }

    if (!info[0]->IsObject())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "removeChild", "Argument must be a Node")));
      return;
    }

    Node *childNode = Unwrap(isolate, info[0]->ToObject(context).ToLocalChecked());
    if (childNode == nullptr || childNode->handle() == nullptr)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "removeChild", "Argument must be a Node")));
      return;
    }

    auto result = handle()->removeChild(childNode->handle());
    if (result != nullptr)
    {
      Local<Object> resultWrapper = Node::GetOrNewInstance(isolate, result);
      info.GetReturnValue().Set(resultWrapper);
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  void Node::InsertBefore(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (info.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "insertBefore", "Illegal arguments")));
      return;
    }

    auto newChildNode = Node::Unwrap(isolate, info[0]->ToObject(context).ToLocalChecked())->handle();
    auto refChildNode = Node::Unwrap(isolate, info[1]->ToObject(context).ToLocalChecked())->handle();
    if (newChildNode == nullptr || refChildNode == nullptr)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "insertBefore", "Arguments must be Nodes")));
      return;
    }

    auto insertedNode = handle()->insertBefore(newChildNode, refChildNode);
    if (insertedNode != nullptr)
    {
      Local<Object> resultWrapper = Node::GetOrNewInstance(isolate, insertedNode);
      info.GetReturnValue().Set(resultWrapper);
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  void Node::ReplaceChild(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (info.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "replaceChild", "replaceChild requires 2 arguments")));
      return;
    }
    if (!info[0]->IsObject() || !info[1]->IsObject())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "replaceChild", "Arguments must be Nodes")));
      return;
    }

    auto newChildNode = Node::Unwrap(isolate, info[0]->ToObject(context).ToLocalChecked())->handle();
    auto oldChildNode = Node::Unwrap(isolate, info[1]->ToObject(context).ToLocalChecked())->handle();
    if (newChildNode == nullptr || oldChildNode == nullptr)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "replaceChild", "Arguments must be Nodes")));
      return;
    }

    auto replacedNode = handle()->replaceChild(newChildNode, oldChildNode);
    if (replacedNode != nullptr)
    {
      Local<Object> resultWrapper = Node::GetOrNewInstance(isolate, replacedNode);
      info.GetReturnValue().Set(resultWrapper);
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  void Node::CloneNode(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    bool deep = false;
    if (info.Length() > 0 && info[0]->IsBoolean())
      deep = info[0]->BooleanValue(isolate);

    auto clonedNode = handle()->cloneNode(deep);
    if (clonedNode != nullptr)
    {
      Local<Object> nodeWrapper = Node::GetOrNewInstance(isolate, clonedNode);
      info.GetReturnValue().Set(nodeWrapper);
    }
    else
    {
      info.GetReturnValue().SetNull();
    }
  }

  void Node::HasChildNodes(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    info.GetReturnValue().Set(Boolean::New(isolate,
                                           handle()->hasChildNodes()));
  }

  void Node::Contains(const FunctionCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    if (info.Length() < 1 || !info[0]->IsObject())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "contains", "Argument must be a Node")));
      return;
    }

    Local<Context> context = isolate->GetCurrentContext();
    Node *otherNode = Unwrap(isolate, info[0]->ToObject(context).ToLocalChecked());
    if (otherNode == nullptr || otherNode->handle() == nullptr)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "contains", "Argument must be a Node")));
      return;
    }

    // TODO: implement contains properly
    info.GetReturnValue().Set(Boolean::New(isolate, false));
  }

  void Node::IsDefaultNamespace(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(Exception::Error(
      MakeMethodError(isolate, "isDefaultNamespace", "Not implemented")));
    return;
  }

  void Node::IsEqualNode(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "isEqualNode", "Requires 1 argument but 0 given")));
      return;
    }

    if (info[0]->IsNull())
    {
      info.GetReturnValue().Set(Boolean::New(isolate, false));
      return;
    }
    else if (!info[0]->IsObject())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "isEqualNode", "Argument must be a Node or null")));
      return;
    }

    auto otherNode = Node::Unwrap(isolate, info[0]->ToObject(context).ToLocalChecked());
    if (otherNode == nullptr || otherNode->handle() == nullptr)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "isEqualNode", "Argument must be a Node or null")));
      return;
    }

    bool isEqual = handle()->isEqualNode(*otherNode->handle());
    info.GetReturnValue().Set(Boolean::New(isolate, isEqual));
  }

  void Node::IsSameNode(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (info.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "isSameNode", "Requires 1 argument but 0 given")));
      return;
    }
    if (!info[0]->IsObject())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "isSameNode", "Argument must be a Node")));
      return;
    }

    // Fast path for the same JS object
    if (info[0]->StrictEquals(info.This()))
    {
      info.GetReturnValue().Set(Boolean::New(isolate, true));
      return;
    }

    auto otherNode = Node::Unwrap(isolate, info[0]->ToObject(context).ToLocalChecked());
    if (otherNode == nullptr || otherNode->handle() == nullptr)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "isSameNode", "Argument must be a Node")));
      return;
    }

    bool isSame = handle()->isSameNode(*otherNode->handle());
    info.GetReturnValue().Set(Boolean::New(isolate, isSame));
  }

  void Node::Normalize(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(Exception::Error(
      MakeMethodError(isolate, "normalize", "Not implemented")));
    return;
  }
}
