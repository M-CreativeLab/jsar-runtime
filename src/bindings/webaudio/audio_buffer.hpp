#pragma once

#include <functional>
#include <napi.h>
#include <LabSound/LabSound.h>
#include "audio_node.hpp"

using namespace std;

namespace webaudio
{
  class AudioBuffer : public Napi::ObjectWrap<AudioBuffer>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    AudioBuffer(const Napi::CallbackInfo &info);
    ~AudioBuffer();

    Napi::Value GetChannelData(const Napi::CallbackInfo &info);
    Napi::Value CopyFromChannel(const Napi::CallbackInfo &info);
    Napi::Value CopyToChannel(const Napi::CallbackInfo &info);
    Napi::Value Load(const Napi::CallbackInfo &info);

    Napi::Value SampleRateGetter(const Napi::CallbackInfo &info);
    Napi::Value LengthGetter(const Napi::CallbackInfo &info);
    Napi::Value DurationGetter(const Napi::CallbackInfo &info);
    Napi::Value NumberOfChannelsGetter(const Napi::CallbackInfo &info);

    void Load(Napi::ArrayBuffer arrayBuffer, size_t byteOffset, size_t byteLength, Napi::Function cbFn);
    static void ProcessLoadInMainThread(AudioBuffer *audioBuffer);

    uint32_t sampleRate;
    Napi::Reference<Napi::Array> buffers;
    Napi::Reference<Napi::Function> cbFn;
    shared_ptr<lab::AudioBus> audioBus;
    string error;

    friend class AudioBufferSourceNode;
    friend class ScriptProcessorNode;
  };

  class AudioBufferSourceNode : public AudioNode
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    AudioBufferSourceNode(const Napi::CallbackInfo &info);
    ~AudioBufferSourceNode();

    Napi::Value Start(const Napi::CallbackInfo &info);
    Napi::Value Stop(const Napi::CallbackInfo &info);

    Napi::Value BufferGetter(const Napi::CallbackInfo &info);
    void BufferSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value OnEndedGetter(const Napi::CallbackInfo &info);
    void OnEndedSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

    static void ProcessInMainThread(AudioBufferSourceNode *self);

    Napi::Reference<Napi::Object> buffer;
    Napi::Reference<Napi::Function> onended;
  };
} // namespace webaudio
