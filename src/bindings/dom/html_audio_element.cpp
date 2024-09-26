#include <assert.h>
#include "./html_audio_element.hpp"

namespace dombinding
{
  Napi::FunctionReference *HTMLAudioElement::constructor;
  vector<Napi::ClassPropertyDescriptor<HTMLAudioElement>> HTMLAudioElement::GetClassProperties()
  {
    auto props = HTMLMediaElementBase<HTMLAudioElement, dom::HTMLAudioElement>::GetClassProperties();
    auto added = vector<Napi::ClassPropertyDescriptor<HTMLAudioElement>>(
        {
            // Audio Properties
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  void HTMLAudioElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "HTMLAudioElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    auto global = env.Global();
    global.Set("HTMLAudioElement", func);
    // global.Set("Audio", Napi::Function::New(env, AudioConstructor));
  }

  Napi::Value HTMLAudioElement::AudioConstructor(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::Object obj = constructor->New({});
    return obj;
  }
}
