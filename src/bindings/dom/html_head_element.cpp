#include <assert.h>
#include "./html_head_element.hpp"

namespace dombinding
{
  Napi::FunctionReference *HTMLHeadElement::constructor;
  void HTMLHeadElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "HTMLHeadElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLHeadElement", func);
  }
}
