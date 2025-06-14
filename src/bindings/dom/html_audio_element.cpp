#include <assert.h>
#include "./html_audio_element.hpp"
#include "./document-inl.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *HTMLAudioElement::constructor;
  vector<Napi::ClassPropertyDescriptor<HTMLAudioElement>> HTMLAudioElement::GetClassProperties(Napi::Env env)
  {
    auto props = HTMLMediaElementBase<HTMLAudioElement, dom::HTMLAudioElement>::GetClassProperties(env);
    auto added = vector<Napi::ClassPropertyDescriptor<HTMLAudioElement>>(
      {
        // Audio Properties
      });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  void HTMLAudioElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, "HTMLAudioElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    auto global = env.Global();
    global.Set("HTMLAudioElement", func);
    global.Set("Audio", Napi::Function::New(env, AudioConstructor));
  }

  Napi::Value HTMLAudioElement::AudioConstructor(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (!info.IsConstructCall())
    {
      auto msg =
        "Failed to construct 'Audio': "
        "Please use the 'new' operator, this object constructor cannot be called as a function.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Null();
    }

    Document *document = Document::GetCurrent(env);
    if (TR_UNLIKELY(document == nullptr))
    {
      auto msg =
        "Failed to construct 'Audio': "
        "The global object 'document' is not an instance of 'Document'.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Null();
    }

    auto documentObject = document->Value();
    auto audioValue = documentObject
                        .Get("createElement")
                        .As<Napi::Function>()
                        .Call(documentObject, {Napi::String::New(env, "audio")});

    auto audioObject = audioValue.As<Napi::Object>();
    if (info.Length() >= 1 && info[0].IsString())
    {
      auto src = info[0].As<Napi::String>().Utf8Value();
      audioObject.Set("src", Napi::String::New(env, src));
    }
    return audioValue;
  }
}
