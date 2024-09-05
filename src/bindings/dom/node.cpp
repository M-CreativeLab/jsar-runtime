#include "./node.hpp"
#include "./element.hpp"

namespace dombinding
{
  Napi::FunctionReference *Node::constructor;

  void Node::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "Node", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("Node", func);
  }

  Napi::Object CreateNode(Napi::Env env, std::shared_ptr<dom::Node> node)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj;
    if (node->nodeType == dom::NodeType::ELEMENT_NODE)
    {
      auto element = dynamic_cast<dom::Element *>(node.get());
      auto external = Napi::External<dom::Element>::New(env, element);
      obj = Element::constructor->New({external});
    }
    else
    {
      auto external = Napi::External<dom::Node>::New(env, node.get());
      obj = Node::constructor->New({external});
    }
    return scope.Escape(obj).ToObject();
  }
}
