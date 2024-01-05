#pragma once

#include <thread>
#include <memory>
#include <deque>
#include <functional>
#include <napi.h>
#include <LabSound/LabSound.h>

#include "audio.hpp"
#include "audio_node.hpp"
#include "audio_source_node.hpp"
#include "audio_destination_node.hpp"
#include "gain_node.hpp"
#include "analyser_node.hpp"
#include "panner_node.hpp"
#include "oscillator_node.hpp"
#include "stereo_panner_node.hpp"
#include "audio_buffer.hpp"
#include "script_processor_node.hpp"
#include "audio_listener.hpp"
#include "media_stream_track.hpp"
#include "microphone_media_stream.hpp"

using namespace std;

namespace webaudio
{
  class WebAudioAsync;

  WebAudioAsync *getWebAudioAsync();
  std::unique_ptr<lab::AudioContext> getNewAudioContext();
  lab::AudioContext *getDefaultAudioContext();
  void disposeDefaultAudioContext();

  class AudioContext : public Napi::ObjectWrap<AudioContext>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    static Napi::Value NewInstance(Napi::Env env, lab::AudioContext *audioContext);
    AudioContext();
    AudioContext(float sampleRate);
    AudioContext(const Napi::CallbackInfo &info);
    ~AudioContext();

    Napi::Value CreateMediaElementSource(const Napi::CallbackInfo &info);
    Napi::Value CreateMediaStreamSource(const Napi::CallbackInfo &info);
    Napi::Value CreateMediaStreamDestination(const Napi::CallbackInfo &info);
    Napi::Value CreateMediaStreamTrackSource(const Napi::CallbackInfo &info);
    Napi::Value CreateGain(const Napi::CallbackInfo &info);
    Napi::Value CreateAnalyser(const Napi::CallbackInfo &info);
    Napi::Value CreatePanner(const Napi::CallbackInfo &info);
    Napi::Value CreateStereoPanner(const Napi::CallbackInfo &info);
    Napi::Value CreateOscillator(const Napi::CallbackInfo &info);
    Napi::Value CreateBuffer(const Napi::CallbackInfo &info);
    Napi::Value CreateEmptyBuffer(const Napi::CallbackInfo &info);
    Napi::Value CreateBufferSource(const Napi::CallbackInfo &info);
    Napi::Value CreateScriptProcessor(const Napi::CallbackInfo &info);
    Napi::Value Suspend(const Napi::CallbackInfo &info);
    Napi::Value Resume(const Napi::CallbackInfo &info);
    Napi::Value Close(const Napi::CallbackInfo &info);

    Napi::Value CurrentTimeGetter(const Napi::CallbackInfo &info);
    Napi::Value SampleRateGetter(const Napi::CallbackInfo &info);

  private:
    static Napi::FunctionReference *constructor;
    std::unique_ptr<lab::AudioContext> audioContext;

    friend class AudioPlayer;
    friend class AudioListener;
    friend class AudioNode;
    friend class AudioSourceNode;
    friend class AudioDestinationNode;
    friend class GainNode;
    friend class AudioParam;
    friend class AudioAnalyser;
    friend class AudioBufferSourceNode;
    friend class ScriptProcessorNode;
  };
}
