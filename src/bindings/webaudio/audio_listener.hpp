#pragma once

#include <napi.h>
#include <LabSound/LabSound.h>
#include "audio_context.hpp"

using namespace std;

namespace webaudio
{
  class AudioListener : public Napi::ObjectWrap<AudioListener>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    AudioListener(const Napi::CallbackInfo &info);
    ~AudioListener();

    Napi::Value SetPosition(const Napi::CallbackInfo &info);
    Napi::Value SetOrientation(const Napi::CallbackInfo &info);

  protected:
    lab::AudioListener *audioListener;
  };
} // namespace webaudio
