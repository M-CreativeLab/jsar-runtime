#include <assert.h>
#include "./html_body_element.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *HTMLBodyElement::constructor;
  void HTMLBodyElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "HTMLBodyElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLBodyElement", func);
  }
}
