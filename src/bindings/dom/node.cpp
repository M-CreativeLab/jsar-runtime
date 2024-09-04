#include "./node.hpp"

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
}
