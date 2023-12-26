#pragma once

#include <string>
#include <napi.h>
#include <LabSound/LabSound.h>
#include "audio_source_node.hpp"

using namespace std;

namespace webaudio
{
  class MediaStreamTrack;
  class MediaStream : public Napi::ObjectWrap<MediaStream>
  {
  protected:
    friend class MediaStreamTrack;
  };

  class MicrophoneMediaStream : public MediaStream
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    MicrophoneMediaStream(const Napi::CallbackInfo &info);
    MicrophoneMediaStream();
    ~MicrophoneMediaStream();

  protected:
    Napi::Value GetTracks(const Napi::CallbackInfo &info);

  protected:
    Napi::Reference<Napi::Array> tracks;

    friend class AudioSourceNode;
  };
} // namespace webaudio
