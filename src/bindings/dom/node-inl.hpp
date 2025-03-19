#pragma once

#include <napi.h>
#include "common/utility.hpp"
#include "./node.hpp"
#include "./element.hpp"
#include "./document.hpp"

namespace dombinding
{
  template <typename ObjectType, typename NodeType>
  bool NodeBase<ObjectType, NodeType>::IsNodeInstance(Napi::Object object)
  {
    return object.Has(NODE_IMPL_FIELD);
  }

  template <typename ObjectType, typename NodeType>
  std::vector<Napi::ClassPropertyDescriptor<ObjectType>> NodeBase<ObjectType, NodeType>::GetClassProperties(Napi::Env env)
  {
    using T = NodeBase<ObjectType, NodeType>;

    auto props = EventTargetWrap<ObjectType, NodeType>::GetClassProperties(env);
    auto added = vector<Napi::ClassPropertyDescriptor<ObjectType>>(
        {
#define _NODE_TYPE_MAP(XX)        \
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

#define XX(TYPE) T::StaticValue(#TYPE, Napi::Number::New(env, static_cast<int>(dom::NodeType::TYPE))), \
                 T::InstanceValue(#TYPE, Napi::Number::New(env, static_cast<int>(dom::NodeType::TYPE))),
            _NODE_TYPE_MAP(XX)
#undef XX
#undef _NODE_TYPE_MAP

            // Getters & Setters
            T::InstanceAccessor(NODE_IMPL_FIELD, &T::NodeImplGetter, nullptr, napi_default),
            T::InstanceAccessor("nodeName", &T::NodeNameGetter, nullptr, napi_default_jsproperty),
            T::InstanceAccessor("nodeType", &T::NodeTypeGetter, nullptr, napi_default_jsproperty),
            T::InstanceAccessor("nodeValue", &T::NodeValueGetter, &T::NodeValueSetter, napi_default_jsproperty),
            T::InstanceAccessor("baseURI", &T::BaseURIGetter, nullptr, napi_default_jsproperty),
            T::InstanceAccessor("isConnected", &T::IsConnectedGetter, nullptr, napi_default_jsproperty),
            T::InstanceAccessor("childNodes", &T::ChildNodesGetter, nullptr, napi_default_jsproperty),
            T::InstanceAccessor("firstChild", &T::FirstChildGetter, nullptr, napi_default_jsproperty),
            T::InstanceAccessor("lastChild", &T::LastChildGetter, nullptr, napi_default_jsproperty),
            T::InstanceAccessor("ownerDocument", &T::OwnerDocumentGetter, nullptr, napi_default_jsproperty),
            T::InstanceAccessor("parentNode", &T::ParentNodeGetter, nullptr, napi_default_jsproperty),
            T::InstanceAccessor("parentElement", &T::ParentElementGetter, nullptr, napi_default_jsproperty),
            T::InstanceAccessor("previousSibling", &T::PreviousSiblingGetter, nullptr, napi_default_jsproperty),
            T::InstanceAccessor("nextSibling", &T::NextSiblingGetter, nullptr, napi_default_jsproperty),
            T::InstanceAccessor("textContent", &T::TextContentGetter, &T::TextContentSetter, napi_default_jsproperty),

            // Methods
            T::InstanceMethod("appendChild", &T::AppendChild),
            T::InstanceMethod("removeChild", &T::RemoveChild),
            T::InstanceMethod("replaceChild", &T::ReplaceChild),
            T::InstanceMethod("cloneNode", &T::CloneNode),
            T::InstanceMethod("compareDocumentPosition", &T::CompareDocumentPosition),
            T::InstanceMethod("contains", &T::Contains),
            T::InstanceMethod("getRootNode", &T::GetRootNode),
            T::InstanceMethod("hasChildNodes", &T::HasChildNodes),
            T::InstanceMethod("insertBefore", &T::InsertBefore),
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::FromImpl(Napi::Env env, shared_ptr<NodeType> node)
  {
    Napi::EscapableHandleScope scope(env);
    NodeContainer<NodeType> nodeContainer(node);
    auto external = Napi::External<NodeContainer<NodeType>>::New(env, &nodeContainer);
    auto instance = ObjectType::constructor->New({external});
    return scope.Escape(instance).ToObject();
  }

  template <typename ObjectType, typename NodeType>
  std::shared_ptr<NodeType> NodeBase<ObjectType, NodeType>::GetImpl(Napi::Value value)
  {
    if (!value.IsObject())
      return nullptr;

    auto valueObject = value.As<Napi::Object>();
    if (!NodeBase::IsNodeInstance(valueObject))
      return nullptr;

    auto nodeImplExternal = valueObject
                                .Get(NODE_IMPL_FIELD)
                                .As<Napi::External<NodeContainer<dom::Node>>>();
    return nodeImplExternal.Data()->node;
  }

  template <typename ObjectType, typename NodeType>
  NodeBase<ObjectType, NodeType>::NodeBase(const Napi::CallbackInfo &info)
      : EventTargetWrap<ObjectType, NodeType>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() >= 1 && info[0].IsExternal())
    {
      NodeContainer<NodeType> *container = info[0].As<Napi::External<NodeContainer<NodeType>>>().Data();
      ResetNode(info, container->node);
    }
    else
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }
  }

  template <typename ObjectType, typename NodeType>
  NodeBase<ObjectType, NodeType>::~NodeBase()
  {
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::NodeImplGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::External<NodeContainer<NodeType>>::New(env, new NodeContainer<NodeType>(node));
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::NodeNameGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, node->nodeName);
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::NodeTypeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, static_cast<int>(node->nodeType));
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::NodeValueGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    // TODO: implement the getter
    Napi::TypeError::New(env, "Failed to get 'nodeValue' property: not implemented").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  void NodeBase<ObjectType, NodeType>::NodeValueSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    // TODO: implement the setter
    Napi::TypeError::New(env, "Failed to set 'nodeValue' property: not implemented").ThrowAsJavaScriptException();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::BaseURIGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, node->baseURI);
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::IsConnectedGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, node->connected);
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::ChildNodesGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto childNodes = node->getChildNodes();
    Napi::Array jsChildNodes = Napi::Array::New(env, childNodes.size());
    for (size_t i = 0; i < childNodes.size(); i++)
      jsChildNodes.Set(i, Node::NewInstance(env, childNodes[i]));
    return jsChildNodes;
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::FirstChildGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    shared_ptr<dom::Node> firstChildNode = node->firstChild();
    if (firstChildNode != nullptr)
      return Node::NewInstance(env, firstChildNode);
    else
      return env.Null();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::LastChildGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    shared_ptr<dom::Node> lastChildNode = node->lastChild();
    if (lastChildNode != nullptr)
      return Node::NewInstance(env, lastChildNode);
    else
      return env.Null();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::OwnerDocumentGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto ownerDocument = node->getOwnerDocumentReference();
    if (ownerDocument != nullptr)
      return Document::NewInstance(env, ownerDocument);
    else
      return env.Null();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::ParentNodeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    std::shared_ptr<dom::Node> parentNode = node->getParentNode();
    if (parentNode != nullptr)
      return Node::NewInstance(env, parentNode);
    else
      return env.Null();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::ParentElementGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    std::shared_ptr<dom::Element> parentElement = node->getParentElement();
    if (parentElement != nullptr)
      return Element::NewInstance(env, parentElement);
    else
      return env.Null();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::PreviousSiblingGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto previousSiblingNode = this->node->previousSibling();
    return previousSiblingNode == nullptr
               ? env.Null()
               : Node::NewInstance(env, previousSiblingNode);
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::NextSiblingGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto nextSiblingNode = this->node->nextSibling();
    return nextSiblingNode == nullptr
               ? env.Null()
               : Node::NewInstance(env, nextSiblingNode);
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::TextContentGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (node->nodeType == dom::NodeType::DOCUMENT_NODE || node->nodeType == dom::NodeType::DOCUMENT_TYPE_NODE)
      return env.Null();
    else
      return Napi::String::New(env, node->textContent());
  }

  template <typename ObjectType, typename NodeType>
  void NodeBase<ObjectType, NodeType>::TextContentSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (node->nodeType == dom::NodeType::DOCUMENT_NODE ||
        node->nodeType == dom::NodeType::DOCUMENT_TYPE_NODE)
      return;

    else if (value.IsString())
      node->textContent(value.As<Napi::String>());
    else
      node->textContent("");
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::AppendChild(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 ||
        !info[0].IsObject() ||
        !IsNodeInstance(info[0].ToObject()))
    {
      Napi::TypeError::New(env, "Illegal arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto jsChildNodeImplExternal = info[0]
                                       .ToObject()
                                       .Get(NODE_IMPL_FIELD)
                                       .As<Napi::External<NodeContainer<dom::Node>>>();
    auto childNodeImpl = jsChildNodeImplExternal.Data()->node;
    node->appendChild(childNodeImpl);
    return info[0].ToObject();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::RemoveChild(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 ||
        !info[0].IsObject() ||
        !IsNodeInstance(info[0].ToObject()))
    {
      Napi::TypeError::New(env, "Illegal arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto jsChildNodeImplExternal = info[0]
                                       .ToObject()
                                       .Get(NODE_IMPL_FIELD)
                                       .As<Napi::External<NodeContainer<dom::Node>>>();
    auto childNodeImpl = jsChildNodeImplExternal.Data()->node;
    node->removeChild(childNodeImpl);
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::ReplaceChild(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2 ||
        !info[0].IsObject() ||
        !IsNodeInstance(info[0].ToObject()) ||
        !info[1].IsObject() ||
        !IsNodeInstance(info[1].ToObject()))
    {
      Napi::TypeError::New(env, "Illegal arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto jsNewChildNodeImplExternal = info[0]
                                          .ToObject()
                                          .Get(NODE_IMPL_FIELD)
                                          .As<Napi::External<NodeContainer<dom::Node>>>();
    auto newChildNodeImpl = jsNewChildNodeImplExternal.Data()->node;

    auto jsOldChildNodeImplExternal = info[1]
                                          .ToObject()
                                          .Get(NODE_IMPL_FIELD)
                                          .As<Napi::External<NodeContainer<dom::Node>>>();
    auto oldChildNodeImpl = jsOldChildNodeImplExternal.Data()->node;

    if (node->replaceChild(newChildNodeImpl, oldChildNodeImpl) == nullptr)
    {
      Napi::TypeError::New(env, "Not found the old child node").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    return info[1].ToObject();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::CloneNode(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    bool deep = false;
    if (info.Length() >= 1 && info[0].IsBoolean())
      deep = info[0].As<Napi::Boolean>();

    auto clonedNode = node->cloneNode(deep);
    return Node::NewInstance(env, clonedNode);
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::CompareDocumentPosition(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::Contains(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::GetRootNode(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::HasChildNodes(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::InsertBefore(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2 ||
        !info[0].IsObject() ||
        !IsNodeInstance(info[0].ToObject()) ||
        !info[1].IsObject() ||
        !IsNodeInstance(info[1].ToObject()))
    {
      Napi::TypeError::New(env, "Illegal arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto newChildNodeImpl = Node::GetImpl(info[0]);
    auto refChildNodeImpl = Node::GetImpl(info[1]);
    if (newChildNodeImpl == nullptr || refChildNodeImpl == nullptr)
    {
      Napi::TypeError::New(env, "Illegal arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto insertedNode = this->node->insertBefore(newChildNodeImpl, refChildNodeImpl);
    return insertedNode == nullptr
               ? env.Undefined()
               : Node::NewInstance(env, insertedNode);
  }

  template <typename ObjectType, typename NodeType>
  void NodeBase<ObjectType, NodeType>::ResetNode(const Napi::CallbackInfo &info, shared_ptr<NodeType> nodeToSet)
  {
    node = nodeToSet;
    this->setEventTarget(node);
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      // Update the static properties which are not be changed
      auto jsThis = info.This().As<Napi::Object>();
      jsThis.Set("baseURI", Napi::String::New(env, node->baseURI));
      jsThis.Set("nodeName", Napi::String::New(env, node->nodeName));
      jsThis.Set("nodeType", Napi::Number::New(env, static_cast<int>(node->nodeType)));
    }
  }
}
