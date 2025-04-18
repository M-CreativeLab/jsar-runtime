#include <assert.h>
#include "./html_video_element.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *HTMLVideoElement::constructor;
  vector<Napi::ClassPropertyDescriptor<HTMLVideoElement>> HTMLVideoElement::GetClassProperties(Napi::Env env)
  {
    // using T = HTMLVideoElement;
    auto props = HTMLMediaElementBase<HTMLVideoElement, dom::HTMLVideoElement>::GetClassProperties(env);
    auto added = vector<Napi::ClassPropertyDescriptor<HTMLVideoElement>>(
        {
            // Video Properties
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  void HTMLVideoElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, "HTMLVideoElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLVideoElement", func);
  }
}
