#pragma once

#include <napi.h>
#include <LabSound/LabSound.h>
#include "audio_node.hpp"
#include "audio_context.hpp"
#include "fake_audio_param.hpp"

using namespace std;

namespace webaudio
{
  class PannerNode : public AudioNode
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    PannerNode(const Napi::CallbackInfo &info);
    ~PannerNode();

    Napi::Value SetPosition(const Napi::CallbackInfo &info);
    Napi::Value SetOrientation(const Napi::CallbackInfo &info);
    Napi::Value SetPath(const Napi::CallbackInfo &info);

    Napi::Value ConeInnerAngleGetter(const Napi::CallbackInfo &info);
    void ConeInnerAngleSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value ConeOuterAngleGetter(const Napi::CallbackInfo &info);
    void ConeOuterAngleSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value DistanceModelGetter(const Napi::CallbackInfo &info);
    void DistanceModelSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value MaxDistanceGetter(const Napi::CallbackInfo &info);
    void MaxDistanceSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value PanningModelGetter(const Napi::CallbackInfo &info);
    void PanningModelSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value RefDistanceGetter(const Napi::CallbackInfo &info);
    void RefDistanceSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value RolloffFactorGetter(const Napi::CallbackInfo &info);
    void RolloffFactorSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
  };
} // namespace webaudio
