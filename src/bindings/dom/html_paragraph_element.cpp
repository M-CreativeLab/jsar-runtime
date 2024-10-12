#include <assert.h>
#include "./html_paragraph_element.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *HTMLParagraphElement::constructor;
  void HTMLParagraphElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "HTMLParagraphElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLParagraphElement", func);
  }
}
