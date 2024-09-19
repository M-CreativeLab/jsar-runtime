#pragma once

#include <memory>
#include <vector>
#include <napi.h>
#include "client/dom/node.hpp"

using namespace std;

namespace dombinding
{
  /**
   * A container for the NodeType instance.
   *
   * Napi::External<T> doesn't support shared_ptr<T> directly, thus we need to wrap it via this container
   * class.
   *
   * @tparam NodeType The node type
   */
  template <typename NodeType>
  class NodeContainer
  {
  public:
    NodeContainer(shared_ptr<NodeType> node) : node(node) {}

  public:
    shared_ptr<NodeType> node;
  };

  template <typename ObjectType, typename NodeType>
  class NodeBase : public Napi::ObjectWrap<ObjectType>
  {
  public:
    static vector<Napi::ClassPropertyDescriptor<ObjectType>> GetClassProperties()
    {
      using T = NodeBase<ObjectType, NodeType>;
      return {
          // Getters & Setters
          T::InstanceAccessor("isConnected", &T::IsConnectedGetter, nullptr),
          T::InstanceAccessor("childNodes", &T::ChildNodesGetter, nullptr),
          T::InstanceAccessor("firstChild", &T::FirstChildGetter, nullptr),
          T::InstanceAccessor("lastChild", &T::LastChildGetter, nullptr),
          // Methods
          T::InstanceMethod("appendChild", &T::AppendChild),
          T::InstanceMethod("cloneNode", &T::CloneNode),
          T::InstanceMethod("compareDocumentPosition", &T::CompareDocumentPosition),
          T::InstanceMethod("contains", &T::Contains),
          T::InstanceMethod("getRootNode", &T::GetRootNode),
          T::InstanceMethod("hasChildNodes", &T::HasChildNodes),
          T::InstanceMethod("insertBefore", &T::InsertBefore),
      };
    }

  public:
    NodeBase(const Napi::CallbackInfo &info);
    virtual ~NodeBase() = default;

  protected: // Getters & Setters
    Napi::Value IsConnectedGetter(const Napi::CallbackInfo &info);
    Napi::Value ChildNodesGetter(const Napi::CallbackInfo &info);
    Napi::Value FirstChildGetter(const Napi::CallbackInfo &info);
    Napi::Value LastChildGetter(const Napi::CallbackInfo &info);
    Napi::Value TextContentGetter(const Napi::CallbackInfo &info);

  protected: // Methods
    Napi::Value AppendChild(const Napi::CallbackInfo &info);
    Napi::Value CloneNode(const Napi::CallbackInfo &info);
    Napi::Value CompareDocumentPosition(const Napi::CallbackInfo &info);
    Napi::Value Contains(const Napi::CallbackInfo &info);
    Napi::Value GetRootNode(const Napi::CallbackInfo &info);
    Napi::Value HasChildNodes(const Napi::CallbackInfo &info);
    Napi::Value InsertBefore(const Napi::CallbackInfo &info);

  protected:
    void ResetNode(const Napi::CallbackInfo &info, shared_ptr<NodeType> nodeToSet);

  protected:
    shared_ptr<NodeType> node = nullptr;
  };

  class Node : public NodeBase<Node, dom::Node>
  {
  public:
    using NodeBase::NodeBase;

  public:
    static void Init(Napi::Env env);

    /**
     * Create a new instance of `Node` in JavaScript from a node-implementation object.
     *
     * @param env The N-API environment
     * @param node The node-implementation object
     * @return The created `Node` instance.
     */
    static Napi::Object NewInstance(Napi::Env env, shared_ptr<dom::Node> node);

  public:
    static Napi::FunctionReference *constructor;
  };
}

#include "./node-inl.hpp"
