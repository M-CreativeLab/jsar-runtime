#pragma once

#include <memory>
#include <napi.h>
#include "client/media/audio_player.hpp"
#include "client/per_process.hpp"

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
      Napi::Value DurationGetter(const Napi::CallbackInfo &info);
      void LoopSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
      Napi::Value LoopGetter(const Napi::CallbackInfo &info);
      void VolumeSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
      Napi::Value VolumeGetter(const Napi::CallbackInfo &info);

    private:
      static Napi::Value OnLoadArrayBuffer(const Napi::CallbackInfo &info);
      static Napi::Value OnMediaEvent(const Napi::CallbackInfo &info);

    private:
      TrClientContextPerProcess *clientContext = nullptr;
      std::shared_ptr<media_client::AudioPlayer> handle = nullptr;
      Napi::ThreadSafeFunction onMediaEvent;

    private:
      static Napi::FunctionReference *constructor;
    };
  }
}
