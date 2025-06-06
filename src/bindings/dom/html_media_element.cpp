#include <assert.h>
#include "./html_media_element-inl.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *HTMLMediaElement::constructor;
  void HTMLMediaElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, "HTMLMediaElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLMediaElement", func);
  }
}
