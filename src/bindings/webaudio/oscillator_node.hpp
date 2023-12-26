#pragma once

#include <napi.h>
#include <LabSound/LabSound.h>
#include "audio_node.hpp"
#include "audio_param.hpp"

using namespace std;

namespace webaudio
{
  class OscillatorNode : public AudioNode
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    OscillatorNode(const Napi::CallbackInfo &info);
    ~OscillatorNode();

    Napi::Value Start(const Napi::CallbackInfo &info);
    Napi::Value Stop(const Napi::CallbackInfo &info);
  };
} // namespace webaudio
