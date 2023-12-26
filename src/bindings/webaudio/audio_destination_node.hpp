#pragma once

#include <napi.h>
#include <LabSound/LabSound.h>

#include "audio.hpp"
#include "audio_node.hpp"

namespace webaudio
{
  class AudioDestinationNode : public Napi::ObjectWrap<AudioDestinationNode>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    AudioDestinationNode(const Napi::CallbackInfo &info);

    Napi::Value MaxChannelCountGetter(const Napi::CallbackInfo &info);
  };
} // namespace webaudio
