#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <thread>
#include <napi.h>
#include <common/utility.hpp>
#include <client/dom/node.hpp>

#include "./event_target-inl.hpp"

using namespace std;

namespace dombinding
{
#define NODE_IMPL_FIELD "_nodeImpl"

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
    std::shared_ptr<NodeType> node;
  };

  template <typename ObjectType, typename NodeType>
  class NodeBase : public EventTargetWrap<ObjectType, NodeType>
  {
  public:
    /**
     * Check if the given object is an instance of `Node`.
     *
     * @returns True if the object is an instance of `Node`, otherwise false.
     */
    static bool IsNodeInstance(Napi::Object object);

    /**
     * Get the class properties of `Node`.
     *
     * @param env The N-API environment
     * @returns The class properties of `Node`.
     */
    static std::vector<Napi::ClassPropertyDescriptor<ObjectType>> GetClassProperties(Napi::Env env);

    /**
     * Create a new instance of `Node` object from a specific `NodeType` type. If you are to create a JavaScript object
     * from a `NodeType` instance and expect the created object to be the corresponding class of the `NodeType`, you
     * should use `Node::NewInstance` instead, which will automatically determine the class by the node type.
     *
     * @param env The N-API environment
     * @param node The node-implementation object
     * @returns The JavaScript object that wraps the given node-implementation object.
     */
    static Napi::Value FromImpl(Napi::Env env, shared_ptr<NodeType> node);

    /**
     * Get the underlying node-implementation object from the given JavaScript object.
     *
     * @param value The JavaScript value
     * @returns The underlying node-implementation object or `nullptr` if the object is not a `Node` instance.
     */
    static std::shared_ptr<NodeType> GetImpl(Napi::Value value);

  public:
    NodeBase(const Napi::CallbackInfo &info);
    virtual ~NodeBase();

  protected: // Getters & Setters
    Napi::Value NodeImplGetter(const Napi::CallbackInfo &info);
    Napi::Value NodeNameGetter(const Napi::CallbackInfo &info);
    Napi::Value NodeTypeGetter(const Napi::CallbackInfo &info);
    Napi::Value NodeValueGetter(const Napi::CallbackInfo &info);
    void NodeValueSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value BaseURIGetter(const Napi::CallbackInfo &info);
    Napi::Value IsConnectedGetter(const Napi::CallbackInfo &info);
    Napi::Value ChildNodesGetter(const Napi::CallbackInfo &info);
    Napi::Value FirstChildGetter(const Napi::CallbackInfo &info);
    Napi::Value LastChildGetter(const Napi::CallbackInfo &info);
    Napi::Value OwnerDocumentGetter(const Napi::CallbackInfo &info);
    Napi::Value ParentNodeGetter(const Napi::CallbackInfo &info);
    Napi::Value ParentElementGetter(const Napi::CallbackInfo &info);
    Napi::Value PreviousSiblingGetter(const Napi::CallbackInfo &info);
    Napi::Value NextSiblingGetter(const Napi::CallbackInfo &info);
    Napi::Value TextContentGetter(const Napi::CallbackInfo &info);
    void TextContentSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

  protected: // Methods
    Napi::Value AppendChild(const Napi::CallbackInfo &info);
    Napi::Value RemoveChild(const Napi::CallbackInfo &info);
    Napi::Value ReplaceChild(const Napi::CallbackInfo &info);
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
     * Create a new instance of `Node` in JavaScript, this will automatically create the corresponding class by the node
     * type. Such as `dom::HTMLDivElement` will be created as `HTMLDivElement`, `dom::Text` will be created as `Text`,
     * etc.
     *
     * @param env The N-API environment
     * @param node The node-implementation object
     * @return The created `Node` instance.
     */
    static Napi::Object NewInstance(Napi::Env env, shared_ptr<dom::Node> node);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
