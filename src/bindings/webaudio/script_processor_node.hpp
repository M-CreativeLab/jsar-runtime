#pragma once

#include <functional>
#include <napi.h>
#include <LabSound/LabSound.h>
#include "audio_node.hpp"

using namespace std;

namespace webaudio
{
  class AudioProcessingEvent : public Napi::ObjectWrap<AudioProcessingEvent>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    AudioProcessingEvent(const Napi::CallbackInfo &info);
    ~AudioProcessingEvent();

    Napi::Value InputBufferGetter(const Napi::CallbackInfo &info);
    Napi::Value OutputBufferGetter(const Napi::CallbackInfo &info);
    Napi::Value NumberOfInputChannelsGetter(const Napi::CallbackInfo &info);
    Napi::Value NumberOfOutputChannelsGetter(const Napi::CallbackInfo &info);

    Napi::Reference<Napi::Object> inputBuffer;
    Napi::Reference<Napi::Object> outputBuffer;
  };

  class ScriptProcessorNode : public AudioNode
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    ScriptProcessorNode(const Napi::CallbackInfo &info);
    ScriptProcessorNode(uint32_t bufferSize, uint32_t numberOfInputChannels, uint32_t numberOfOutputChannels);
    ~ScriptProcessorNode();

    Napi::Value OnAudioProcessGetter(const Napi::CallbackInfo &info);
    Napi::Value OnAudioProcessSetter(const Napi::CallbackInfo &info);

    void ProcessInAudioThread(lab::ContextRenderLock &r, vector<const float *> sources, vector<float *> destinations, size_t framesToProcess);
    static void ProcessInMainThread(ScriptProcessorNode *self);

    uint32_t bufferSize;
    uint32_t numberOfInputChannels;
    uint32_t numberOfOutputChannels;
    size_t bufferIndex;
    Napi::Reference<Napi::Function> audioBufferConstructor;
    Napi::Reference<Napi::Function> audioProcessingEventConstructor;
    Napi::Reference<Napi::Function> onAudioProcess;
    vector<vector<float>> inputBuffers;
    vector<vector<float>> outputBuffers;
  };
} // namespace webaudio
