#pragma once

#include <napi.h>
#include <LabSound/LabSound.h>

#include "audio.hpp"
#include "audio_node.hpp"

namespace webaudio
{
  class AudioSourceNode : public Napi::ObjectWrap<AudioSourceNode>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    AudioSourceNode(const Napi::CallbackInfo &info);
  };
} // namespace webaudio
