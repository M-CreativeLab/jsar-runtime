#include <assert.h>
#include "./html_link_element.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *HTMLLinkElement::constructor;
  void HTMLLinkElement::Init(Napi::Env env)
  {
#define MODULE_NAME "HTMLLinkElement"
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, MODULE_NAME, props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(MODULE_NAME, func);
#undef MODULE_NAME
  }
}
