#pragma once

#include <memory>
#include <napi.h>
#include "client/media/audio_player.hpp"

namespace bindings
{
  namespace dom
  {
    class Audio : public Napi::ObjectWrap<Audio>
    {
    public:
      static void Init(Napi::Env env, Napi::Object exports);
      Audio(const Napi::CallbackInfo &info);
      ~Audio();

    private:
      Napi::Value CanPlayType(const Napi::CallbackInfo &info);
      Napi::Value FastSeek(const Napi::CallbackInfo &info);
      Napi::Value Load(const Napi::CallbackInfo &info);
      Napi::Value Pause(const Napi::CallbackInfo &info);
      Napi::Value Play(const Napi::CallbackInfo &info);

    private:
      static Napi::Value OnLoadArrayBuffer(const Napi::CallbackInfo &info);

    private:
      std::unique_ptr<media::AudioPlayer> handle = nullptr;

    private:
      static Napi::FunctionReference *constructor;
    };
  }
}
