#include <assert.h>
#include "./html_element-inl.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *HTMLElement::constructor;
  void HTMLElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, "HTMLElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLElement", func);
  }
}
