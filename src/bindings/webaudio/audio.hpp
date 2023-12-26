#pragma once

#include <memory>
#include <napi.h>
#include <LabSound/LabSound.h>
#include "audio_context.hpp"

using namespace std;

namespace webaudio
{
  enum SupportedAudioFormat
  {
    MP3,
    WAV,
    OGG,
    UNKNOWN
  };

  class AudioContext;
  class AudioPlayer : public Napi::ObjectWrap<AudioPlayer>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    AudioPlayer(const Napi::CallbackInfo &info);
    ~AudioPlayer();

    void Load(Napi::Env env, uint8_t *bufferValue, size_t bufferLength, Napi::Function cbFn);
    void Play(float when = 0.0f);
    void Pause();
    lab::SampledAudioNode *GetLocalAudioNode();

  public:
    static Napi::FunctionReference *constructor;

  protected:
    Napi::Value Load(const Napi::CallbackInfo &info);
    Napi::Value Play(const Napi::CallbackInfo &info);
    Napi::Value Pause(const Napi::CallbackInfo &info);
    Napi::Value CanPlayType(const Napi::CallbackInfo &info);
    Napi::Value Dispose(const Napi::CallbackInfo &info);

    Napi::Value PausedGetter(const Napi::CallbackInfo &info);
    Napi::Value CurrentTimeGetter(const Napi::CallbackInfo &info);
    void CurrentTimeSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value DurationGetter(const Napi::CallbackInfo &info);
    Napi::Value VolumeGetter(const Napi::CallbackInfo &info);
    void VolumeSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value LoopGetter(const Napi::CallbackInfo &info);
    void LoopSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value OnEndedGetter(const Napi::CallbackInfo &info);
    void OnEndedSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

    void SetupAudioNodeAndBus();
    static void ProcessLoadInMainThread(Napi::Env env, Napi::Function callback, AudioPlayer *self);
    static void ProcessEndedInMainThread(Napi::Env env, Napi::Function callback, AudioPlayer *self);

    Napi::FunctionReference cbFn;
    Napi::FunctionReference onended;

  protected:
    bool loaded;
    bool sourced;
    bool looped;
    float volume;
    lab::AudioContext *sourceAudioContext;
    shared_ptr<lab::SampledAudioNode> audioNode;
    shared_ptr<lab::SampledAudioNode> sourceAudioNode;
    shared_ptr<lab::GainNode> gainNode;
    Napi::ThreadSafeFunction onloadedTSFN;
    Napi::ThreadSafeFunction onendedTSFN;
    shared_ptr<lab::AudioBus> audioBus;
    std::thread loadingThread;
    std::string error;

    friend class AudioSourceNode;
  };

  float GetDurationOnSampledAudioNode(lab::SampledAudioNode *node);
  SupportedAudioFormat DetectAudioFormat(std::vector<unsigned char> audioBuffer);

  Napi::Object InitWebAudioModule(Napi::Env env, Napi::Object exports);
} // namespace webaudio
