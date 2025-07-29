#include <assert.h>
#include "./html_iframe_element.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *HTMLIframeElement::constructor;
  void HTMLIframeElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, "HTMLIFrameElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLIFrameElement", func);
  }
}