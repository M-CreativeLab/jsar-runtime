#pragma once

#include <memory>
#include <vector>
#include <napi.h>
#include "client/dom/node.hpp"

using namespace std;

namespace dombinding
{
  class Node;

  template <typename ObjectType, typename NodeType>
  class NodeBase : public Napi::ObjectWrap<ObjectType>
  {
  public:
    static vector<Napi::ClassPropertyDescriptor<ObjectType>> GetClassProperties()
    {
      return {
          // Getters & Setters
          NodeBase<ObjectType, NodeType>::InstanceAccessor("firstChild", &NodeBase<ObjectType, NodeType>::FirstChildGetter, nullptr),
          // Methods
          NodeBase<ObjectType, NodeType>::InstanceMethod("appendChild", &NodeBase<ObjectType, NodeType>::AppendChild),
          NodeBase<ObjectType, NodeType>::InstanceMethod("cloneNode", &NodeBase<ObjectType, NodeType>::CloneNode),
          NodeBase<ObjectType, NodeType>::InstanceMethod("compareDocumentPosition", &NodeBase<ObjectType, NodeType>::CompareDocumentPosition),
          NodeBase<ObjectType, NodeType>::InstanceMethod("contains", &NodeBase<ObjectType, NodeType>::Contains),
          NodeBase<ObjectType, NodeType>::InstanceMethod("getRootNode", &NodeBase<ObjectType, NodeType>::GetRootNode),
          NodeBase<ObjectType, NodeType>::InstanceMethod("hasChildNodes", &NodeBase<ObjectType, NodeType>::HasChildNodes),
          NodeBase<ObjectType, NodeType>::InstanceMethod("insertBefore", &NodeBase<ObjectType, NodeType>::InsertBefore),
      };
    }
    static Napi::Object NewInstance(Napi::Env env, shared_ptr<NodeType> node)
    {
      Napi::EscapableHandleScope scope(env);
      Napi::External<dom::Node> external = Napi::External<dom::Node>::New(env, node.get());
      Napi::Object obj = ObjectType::constructor->New({external});
      return scope.Escape(obj).ToObject();
    }

  public:
    NodeBase(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ObjectType>(info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      if (info.Length() >= 1 && info[0].IsExternal())
      {
        NodeType *data = info[0].As<Napi::External<NodeType>>().Data();
        ResetNode(info, data);
      }
      else
      {
        Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
        return;
      }
    }
    virtual ~NodeBase() = default;

  protected: // Getters & Setters
    Napi::Value FirstChildGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      shared_ptr<dom::Node> firstChildNode = node->getFirstChild();
      if (firstChildNode != nullptr)
        return NodeBase<Node, dom::Node>::NewInstance(env, firstChildNode);
      else
        return env.Null();
    }

  protected: // Methods
    Napi::Value AppendChild(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }
    Napi::Value CloneNode(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }
    Napi::Value CompareDocumentPosition(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }
    Napi::Value Contains(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }
    Napi::Value GetRootNode(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }
    Napi::Value HasChildNodes(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }
    Napi::Value InsertBefore(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }

  protected:
    void ResetNode(const Napi::CallbackInfo &info, NodeType *nodeToSet)
    {
      node = make_shared<NodeType>(*nodeToSet);
      {
        Napi::Env env = info.Env();
        Napi::HandleScope scope(env);

        // Update the static properties which are not be changed
        auto jsThis = info.This().As<Napi::Object>();
        jsThis.Set("baseURI", Napi::String::New(env, node->baseURI));
        jsThis.Set("nodeName", Napi::String::New(env, node->nodeName));
        jsThis.Set("isConnected", Napi::Boolean::New(env, node->connected));
        jsThis.Set("nodeType", Napi::Number::New(env, static_cast<int>(node->nodeType)));
      }
    }

  protected:
    shared_ptr<NodeType> node;
  };

  class Node : public NodeBase<Node, dom::Node>
  {
  public:
    using NodeBase::NodeBase;

  public:
    static void Init(Napi::Env env);

  public:
    static Napi::FunctionReference *constructor;
  };
}
