#include <assert.h>
#include "./html_section_element.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *HTMLSectionElement::constructor;
  void HTMLSectionElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, "HTMLSectionElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLSectionElement", func);
  }
}
