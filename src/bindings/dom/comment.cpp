#include "./comment.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *Comment::constructor;
  void Comment::Init(Napi::Env env)
  {
#define MODULE_NAME "Comment"
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, MODULE_NAME, props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(MODULE_NAME, func);
#undef MODULE_NAME
  }

  Napi::Value Comment::NewInstance(Napi::Env env, std::shared_ptr<dom::Node> nodeImpl)
  {
    auto commentImpl = dynamic_pointer_cast<dom::Comment>(nodeImpl);
    if (commentImpl != nullptr)
      return Comment::FromImpl(env, commentImpl);

    // Fallback to the `Null` if the TEXT_NODE failed to create in the above cases.
    return env.Null();
  }
}
