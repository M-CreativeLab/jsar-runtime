#include <assert.h>
#include "./html_span_element.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *HTMLSpanElement::constructor;
  void HTMLSpanElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "HTMLSpanElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLSpanElement", func);
  }
}
