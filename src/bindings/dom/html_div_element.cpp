#include <assert.h>
#include "./html_div_element.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *HTMLDivElement::constructor;
  void HTMLDivElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "HTMLDivElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLDivElement", func);
  }
}
