#include <memory>
#include <thread>
#include <LabSound/LabSound.h>
#include "audio_context.hpp"

namespace webaudio
{
  thread_local unique_ptr<lab::AudioContext> _defaultAudioContext;
  unique_ptr<lab::AudioContext> getNewAudioContext()
  {
    auto inputConfig = lab::GetDefaultInputAudioDeviceConfiguration();
    auto outputConfig = lab::GetDefaultOutputAudioDeviceConfiguration();
    return lab::MakeRealtimeAudioContext(outputConfig, inputConfig);
  }

  lab::AudioContext *getDefaultAudioContext()
  {
    if (!_defaultAudioContext)
    {
      _defaultAudioContext = getNewAudioContext();
      atexit([]()
             { _defaultAudioContext.reset(); });
    }
    return _defaultAudioContext.get();
  }

  void disposeDefaultAudioContext()
  {
    if (_defaultAudioContext)
      _defaultAudioContext.reset();
  }

  Napi::FunctionReference *AudioContext::constructor;
  void AudioContext::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::HandleScope scope(env);
    Napi::Function tpl = DefineClass(env, "AudioContext", {
                                                              InstanceMethod("createMediaElementSource", &AudioContext::CreateMediaElementSource),
                                                              InstanceMethod("createMediaStreamSource", &AudioContext::CreateMediaStreamSource),
                                                              InstanceMethod("createMediaStreamDestination", &AudioContext::CreateMediaStreamDestination),
                                                              InstanceMethod("createMediaStreamTrackSource", &AudioContext::CreateMediaStreamTrackSource),
                                                              InstanceMethod("createGain", &AudioContext::CreateGain),
                                                              InstanceMethod("createAnalyser", &AudioContext::CreateAnalyser),
                                                              InstanceMethod("createPanner", &AudioContext::CreatePanner),
                                                              InstanceMethod("createStereoPanner", &AudioContext::CreateStereoPanner),
                                                              InstanceMethod("createOscillator", &AudioContext::CreateOscillator),
                                                              InstanceMethod("createBuffer", &AudioContext::CreateBuffer),
                                                              InstanceMethod("createEmptyBuffer", &AudioContext::CreateEmptyBuffer),
                                                              InstanceMethod("createBufferSource", &AudioContext::CreateBufferSource),
                                                              InstanceMethod("createScriptProcessor", &AudioContext::CreateScriptProcessor),
                                                              InstanceMethod("suspend", &AudioContext::Suspend),
                                                              InstanceMethod("resume", &AudioContext::Resume),
                                                              InstanceMethod("close", &AudioContext::Close),
                                                              InstanceAccessor("currentTime", &AudioContext::CurrentTimeGetter, nullptr),
                                                              InstanceAccessor("sampleRate", &AudioContext::SampleRateGetter, nullptr),
                                                          });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);

    exports.Set("AudioContext", tpl);
  }

  AudioContext::AudioContext(const Napi::CallbackInfo &info) : Napi::ObjectWrap<AudioContext>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    audioContext = lab::MakeRealtimeAudioContext(
        lab::GetDefaultOutputAudioDeviceConfiguration(),
        lab::GetDefaultInputAudioDeviceConfiguration());
  }
  AudioContext::~AudioContext() {}

  Napi::Value AudioContext::CreateMediaElementSource(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

  Napi::Value AudioContext::CreateMediaStreamSource(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

  Napi::Value AudioContext::CreateMediaStreamDestination(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::TypeError::New(env, "AudioContext::CreateMediaStreamDestination: not implemented").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::Value AudioContext::CreateMediaStreamTrackSource(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::TypeError::New(env, "AudioContext::CreateMediaStreamTrackSource: not implemented").ThrowAsJavaScriptException();
    return env.Undefined();
  }

  Napi::Value AudioContext::CreateGain(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

  Napi::Value AudioContext::CreateAnalyser(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

  Napi::Value AudioContext::CreatePanner(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

  Napi::Value AudioContext::CreateStereoPanner(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

  Napi::Value AudioContext::CreateOscillator(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

  Napi::Value AudioContext::CreateBuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

  Napi::Value AudioContext::CreateEmptyBuffer(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

  Napi::Value AudioContext::CreateBufferSource(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

  Napi::Value AudioContext::CreateScriptProcessor(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

  Napi::Value AudioContext::Suspend(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

  Napi::Value AudioContext::Resume(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

  Napi::Value AudioContext::Close(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

  Napi::Value AudioContext::CurrentTimeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

  Napi::Value AudioContext::SampleRateGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return env.Undefined();
  }

} // namespace webaudio
