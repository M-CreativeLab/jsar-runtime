#pragma once

#include <client/html/html_audio_element.hpp>
#include "./html_media_element-inl.hpp"

namespace dombinding
{
  class HTMLAudioElement : public HTMLMediaElementBase<HTMLAudioElement, dom::HTMLAudioElement>
  {
  public:
    using HTMLMediaElementBase::HTMLMediaElementBase;

  public:
    static vector<Napi::ClassPropertyDescriptor<HTMLAudioElement>> GetClassProperties(Napi::Env env);
    static void Init(Napi::Env env);
    static Napi::Value AudioConstructor(const Napi::CallbackInfo &info);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
