#pragma once

#include <memory>
#include <napi.h>
#include <LabSound/LabSound.h>

#include "audio_context.hpp"
#include "microphone_media_stream.hpp"

using namespace std;

namespace webaudio
{
  class MediaStream;
  class MediaStreamTrack : public Napi::ObjectWrap<MediaStreamTrack>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    MediaStreamTrack(const Napi::CallbackInfo &info);
    MediaStreamTrack(MediaStream *mediaStream);
    ~MediaStreamTrack();

    Napi::Value Stop(const Napi::CallbackInfo &info);

    Napi::Value EnabledGetter(const Napi::CallbackInfo &info);
    Napi::Value KindGetter(const Napi::CallbackInfo &info);
    Napi::Value LabelGetter(const Napi::CallbackInfo &info);
    Napi::Value MutedGetter(const Napi::CallbackInfo &info);
    void MutedSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value ReadyStateGetter(const Napi::CallbackInfo &info);

  protected:
    // TODO
    // shared_ptr<lab::AudioHardwareSourceNode> audioNode;
    MediaStream *mediaStream;
    bool live;
    bool muted;

    friend class AudioSourceNode;
  };
} // namespace webaudio
