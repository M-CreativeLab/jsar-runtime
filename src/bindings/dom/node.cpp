#include "./node-inl.hpp"
#include "./text.hpp"
#include "./element-inl.hpp"
#include "./document-inl.hpp"
#include "./document_fragment.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *Node::constructor;
  void Node::Init(Napi::Env env)
  {
#define MODULE_NAME "Node"
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, MODULE_NAME, props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(MODULE_NAME, func);
#undef MODULE_NAME
  }

  Napi::Object Node::NewInstance(Napi::Env env, std::shared_ptr<dom::Node> node)
  {
    if (node->nodeType == dom::NodeType::DOCUMENT_NODE)
      return Document::NewInstance(env, node).ToObject();
    if (node->nodeType == dom::NodeType::DOCUMENT_FRAGMENT_NODE)
      return DocumentFragment::NewInstance(env, node).ToObject();
    if (node->nodeType == dom::NodeType::TEXT_NODE)
      return Text::NewInstance(env, node).ToObject();
    if (node->nodeType == dom::NodeType::ELEMENT_NODE)
      return Element::NewInstance(env, node).ToObject();

    // TODO: support other types of nodes, such as `Document`, `DocumentFragment`, etc.
    // Fallback to the `Node` instance
    return Node::FromImpl(env, node).ToObject();
  }
}
