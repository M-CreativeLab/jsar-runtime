#pragma once

#include <napi.h>
#include <LabSound/LabSound.h>
#include "audio_node.hpp"
#include "audio_param.hpp"

using namespace std;

namespace webaudio
{
  class StereoPannerNode : public AudioNode
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    StereoPannerNode(const Napi::CallbackInfo &info);
    ~StereoPannerNode();
  };
} // namespace webaudio
