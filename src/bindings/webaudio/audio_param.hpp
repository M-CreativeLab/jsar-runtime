#pragma once

#include <string>
#include <napi.h>
#include <LabSound/LabSound.h>

#include "audio_context.hpp"

using namespace std;

namespace webaudio
{
  class AudioParam : public Napi::ObjectWrap<AudioParam>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    AudioParam(const Napi::CallbackInfo &info);
    ~AudioParam();

    Napi::Value Connect(const Napi::CallbackInfo &info);
    Napi::Value SetValueAtTime(const Napi::CallbackInfo &info);
    Napi::Value LinearRampToValueAtTime(const Napi::CallbackInfo &info);
    Napi::Value ExponentialRampToValueAtTime(const Napi::CallbackInfo &info);
    Napi::Value SetTargetAtTime(const Napi::CallbackInfo &info);
    Napi::Value SetValueCurveAtTime(const Napi::CallbackInfo &info);
    Napi::Value CancelScheduledValues(const Napi::CallbackInfo &info);
    Napi::Value CancelAndHoldAtTime(const Napi::CallbackInfo &info);

    Napi::Value DefaultValueGetter(const Napi::CallbackInfo &info);
    Napi::Value MaxValueGetter(const Napi::CallbackInfo &info);
    Napi::Value MinValueGetter(const Napi::CallbackInfo &info);
    Napi::Value ValueGetter(const Napi::CallbackInfo &info);
    void ValueSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

  protected:
    Napi::Reference<Napi::Object> context;
    shared_ptr<lab::AudioParam> audioParam;

    friend class GainNode;
    friend class StereoPannerNode;
    friend class OscillatorNode;
    friend class AudioBufferSourceNode;
  };
} // namespace webaudio
