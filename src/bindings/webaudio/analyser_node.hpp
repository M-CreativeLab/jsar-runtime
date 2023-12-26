#pragma once

#include <napi.h>
#include <LabSound/LabSound.h>
#include "audio_node.hpp"
#include "audio_param.hpp"

using namespace std;

namespace webaudio
{
  class AnalyserNode : public AudioNode
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    AnalyserNode(const Napi::CallbackInfo &info);
    ~AnalyserNode();

    Napi::Value GetFloatFrequencyData(const Napi::CallbackInfo &info);
    Napi::Value GetByteFrequencyData(const Napi::CallbackInfo &info);
    Napi::Value GetFloatTimeDomainData(const Napi::CallbackInfo &info);
    Napi::Value GetByteTimeDomainData(const Napi::CallbackInfo &info);

    Napi::Value FftSizeGetter(const Napi::CallbackInfo &info);
    void FftSizeSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value FrequencyBinCountGetter(const Napi::CallbackInfo &info);
    Napi::Value MinDecibelsGetter(const Napi::CallbackInfo &info);
    Napi::Value MaxDecibelsGetter(const Napi::CallbackInfo &info);
    Napi::Value SmoothingTimeConstantGetter(const Napi::CallbackInfo &info);
  };
} // namespace webaudio
