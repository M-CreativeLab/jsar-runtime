#include <assert.h>
#include "./html_media_element.hpp"

namespace dombinding
{
  Napi::FunctionReference *HTMLMediaElement::constructor;
  void HTMLMediaElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "HTMLMediaElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLMediaElement", func);
  }
}
