#pragma once

#include <napi.h>
#include <LabSound/LabSound.h>

#include "audio.hpp"
#include "audio_node.hpp"

namespace webaudio
{
  class GainNode : public Napi::ObjectWrap<GainNode>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    GainNode(const Napi::CallbackInfo &info);
    ~GainNode();
  };
} // namespace webaudio
