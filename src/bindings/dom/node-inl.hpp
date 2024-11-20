#pragma once

#include <napi.h>
#include "common/utility.hpp"
#include "./node.hpp"

namespace dombinding
{
  template <typename ObjectType, typename NodeType>
  bool NodeBase<ObjectType, NodeType>::IsNodeInstance(Napi::Object object)
  {
    return object.Has(NODE_IMPL_FIELD);
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

    shared_ptr<dom::Node> firstChildNode = node->getFirstChild();
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

    shared_ptr<dom::Node> lastChildNode = node->getLastChild();
    if (lastChildNode != nullptr)
      return Node::NewInstance(env, lastChildNode);
    else
      return env.Null();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::TextContentGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (node->nodeType == dom::NodeType::DOCUMENT_NODE || node->nodeType == dom::NodeType::DOCUMENT_TYPE_NODE)
      return env.Null();
    else
      return Napi::String::New(env, node->getTextContent());
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

    auto jsChildNodeImplExternal = info[0].ToObject().Get(NODE_IMPL_FIELD).As<Napi::External<NodeContainer<dom::Node>>>();
    auto childNodeImpl = jsChildNodeImplExternal.Data()->node;
    node->appendChild(childNodeImpl);
    return info[0].ToObject();
  }

  template <typename ObjectType, typename NodeType>
  Napi::Value NodeBase<ObjectType, NodeType>::CloneNode(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
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
    return env.Undefined();
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
