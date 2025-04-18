#include <assert.h>
#include "./html_head_element.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *HTMLHeadElement::constructor;
  void HTMLHeadElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, "HTMLHeadElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLHeadElement", func);
  }
}
