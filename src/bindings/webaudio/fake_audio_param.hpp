#pragma once

#include <functional>
#include <napi.h>
#include <LabSound/LabSound.h>

using namespace std;

namespace webaudio
{
  class FakeAudioParam : public Napi::ObjectWrap<FakeAudioParam>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    FakeAudioParam(const Napi::CallbackInfo &info);
    ~FakeAudioParam();

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
    function<float()> getter;
    function<void(float)> setter;

    friend class AudioListener;
    friend class PannerNode;
  };
} // namespace webaudio
