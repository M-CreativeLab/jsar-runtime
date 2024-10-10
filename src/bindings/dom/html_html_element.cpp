#include <assert.h>
#include "./html_html_element.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *HTMLHtmlElement::constructor;
  void HTMLHtmlElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "HTMLHtmlElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLHtmlElement", func);
  }
}
