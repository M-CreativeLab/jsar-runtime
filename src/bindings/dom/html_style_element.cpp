#include <assert.h>
#include "./html_style_element.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *HTMLStyleElement::constructor;
  void HTMLStyleElement::Init(Napi::Env env)
  {
#define MODULE_NAME "HTMLStyleElement"
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, MODULE_NAME, props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(MODULE_NAME, func);
  }
}
