#include "./node-inl.hpp"
#include "./element.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *Node::constructor;
  void Node::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "Node", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("Node", func);
  }

  Napi::Object Node::NewInstance(Napi::Env env, std::shared_ptr<dom::Node> node)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj;
    if (node->nodeType == dom::NodeType::ELEMENT_NODE)
    {
      obj = Element::NewInstance(env, node);
    }
    else
    {
      NodeContainer nodeContainer(node);
      auto external = Napi::External<NodeContainer<dom::Node>>::New(env, &nodeContainer);
      obj = Node::constructor->New({external});
    }
    return scope.Escape(obj).ToObject();
  }
}
