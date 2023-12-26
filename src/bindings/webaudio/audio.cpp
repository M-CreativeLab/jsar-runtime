#include <memory>
#include <cmath>
#include <algorithm>
#include <node.h>
#include <v8.h>

#include "audio.hpp"
#include "audio_node.hpp"

namespace webaudio
{
  Napi::FunctionReference *AudioPlayer::constructor;
  void AudioPlayer::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(
        env,
        "AudioPlayer",
        {
            InstanceMethod("load", &AudioPlayer::Load),
            InstanceMethod("play", &AudioPlayer::Play),
            InstanceMethod("pause", &AudioPlayer::Pause),
            InstanceMethod("canPlayType", &AudioPlayer::CanPlayType),
            InstanceMethod("dispose", &AudioPlayer::Dispose),
            InstanceAccessor("paused", &AudioPlayer::PausedGetter, nullptr),
            InstanceAccessor("currentTime", &AudioPlayer::CurrentTimeGetter, &AudioPlayer::CurrentTimeSetter),
            InstanceAccessor("duration", &AudioPlayer::DurationGetter, nullptr),
            InstanceAccessor("volume", &AudioPlayer::VolumeGetter, &AudioPlayer::VolumeSetter),
            InstanceAccessor("loop", &AudioPlayer::LoopGetter, &AudioPlayer::LoopSetter),
            InstanceAccessor("onended", &AudioPlayer::OnEndedGetter, &AudioPlayer::OnEndedSetter),
        });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);

    exports.Set("AudioPlayer", tpl);
  }

  AudioPlayer::AudioPlayer(const Napi::CallbackInfo &info) : Napi::ObjectWrap<AudioPlayer>(info),
                                                             sourceAudioContext(nullptr),
                                                             loaded(false),
                                                             sourced(false),
                                                             looped(false),
                                                             volume(1.0f)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    /**
     * Initialize audioNode, sourceAudioNode and gainNode by the default audio context.
     */
    auto ac = getDefaultAudioContext();
    audioNode.reset(new lab::SampledAudioNode(*ac));
    sourceAudioNode.reset(new lab::SampledAudioNode(*ac));
    gainNode.reset(new lab::GainNode(*ac));
  }

  AudioPlayer::~AudioPlayer()
  {
    /**
     * FIXME: We need to join the thread to avoid a crash by thread.
     */
    if (loadingThread.joinable())
      loadingThread.join();
  }

  void AudioPlayer::Load(Napi::Env env, uint8_t *bufferValue, size_t bufferLength, Napi::Function cbFn)
  {
    Napi::HandleScope scope(env);

    if (this->cbFn.IsEmpty())
    {
      this->onloadedTSFN = Napi::ThreadSafeFunction::New(env, cbFn, "Audio::Load", 0, 1);
      this->cbFn.Reset(cbFn);

      std::vector<unsigned char> buffer(bufferLength);
      memcpy(buffer.data(), bufferValue, bufferLength);

      if (loadingThread.joinable())
        loadingThread.join();
      loadingThread = std::thread([this, buffer{std::move(buffer)}]()
                                  {
                                    SupportedAudioFormat format = DetectAudioFormat(buffer);
                                    switch (format)
                                    {
                                    case MP3:
                                      this->audioBus = lab::MakeBusFromMemory(buffer, "mp3", false);
                                      break;
                                    case OGG:
                                      this->audioBus = lab::MakeBusFromMemory(buffer, "ogg", false);
                                      break;
                                    case WAV:
                                    default:
                                      this->audioBus = lab::MakeBusFromMemory(buffer, false);
                                      break;
                                    }
                                    napi_status status = this->onloadedTSFN.BlockingCall(this, ProcessLoadInMainThread);
                                    if (status != napi_ok)
                                      fprintf(stderr, "Audio::Load() tsfn.BlockingCall() failed\n"); });
    }
    else
    {
      cbFn.As<Napi::Function>().Call({Napi::String::New(env, "Audio is loading")});
    }
  }

  void AudioPlayer::Play(float when)
  {
    audioNode->start(0.0f, when, looped ? -1 : 0);
    sourceAudioNode->start(0.0f, when, looped ? -1 : 0);
    /**
     * FIXME: We need this to mark this object can't be released util the runtime is stopped or disposed by manual when the `ownerDocument`
     * is also stopped.
     */
    Ref();
  }

  void AudioPlayer::Pause()
  {
    audioNode->stop(0.0f);
    sourceAudioNode->stop(0.0f);
  }

  lab::SampledAudioNode *AudioPlayer::GetLocalAudioNode()
  {
    if (sourced)
      return sourceAudioNode.get();
    else
      return audioNode.get();
  }

  Napi::Value AudioPlayer::Load(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "Audio.load() takes 2 arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (!info[1].IsFunction())
    {
      Napi::TypeError::New(env, "Audio.load() argument 2 should be a function").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::ArrayBuffer ab;
    Napi::Function cbFn = info[1].As<Napi::Function>();
    if (info[0].IsArrayBuffer())
    {
      ab = info[0].As<Napi::ArrayBuffer>();
    }
    else if (info[0].IsTypedArray())
    {
      ab = info[0].As<Napi::TypedArray>().ArrayBuffer();
    }
    else
    {
      Napi::TypeError::New(env, "Audio.load() argument 1 should be an ArrayBuffer or TypedArray").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Load(env, (uint8_t *)ab.Data(), ab.ByteLength(), cbFn);
    return env.Undefined();
  }

  Napi::Value AudioPlayer::Play(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (!loaded)
    {
      Napi::TypeError::New(env, "The audio is not loaded").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    float when = 0.0f;
    if (info.Length() > 0 && info[0].IsNumber())
    {
      when = info[0].As<Napi::Number>().FloatValue();
    }
    Play(when);
    return env.Undefined();
  }

  Napi::Value AudioPlayer::Pause(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Pause();
    return env.Undefined();
  }

  Napi::Value AudioPlayer::CanPlayType(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsString())
    {
      Napi::TypeError::New(env, "Audio.canPlayType() takes 1 argument").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::string expectTypeStr = info[0].As<Napi::String>().Utf8Value();
    if (expectTypeStr == "audio/mpeg" || expectTypeStr == "audio/mp3" || expectTypeStr == "audio/wav" || expectTypeStr == "audio/ogg")
      return Napi::String::New(env, "probably");
    else
      return Napi::String::New(env, "");
  }

  /**
   * This method should be called manually when the `ownerDocument` is stopped or disposed.
   */
  Napi::Value AudioPlayer::Dispose(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    onended.Reset();
    onendedTSFN.Unref(env);
    onendedTSFN.Release();
    Unref();

    return env.Undefined();
  }

  Napi::Value AudioPlayer::PausedGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return Napi::Boolean::New(env, !GetLocalAudioNode()->isPlayingOrScheduled());
  }

  Napi::Value AudioPlayer::CurrentTimeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    double now = getDefaultAudioContext()->currentTime();
    return Napi::Number::New(env, now);
  }

  void AudioPlayer::CurrentTimeSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (value.IsNumber())
      Play(value.As<Napi::Number>().FloatValue());
  }

  Napi::Value AudioPlayer::DurationGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return Napi::Number::New(env, GetDurationOnSampledAudioNode(GetLocalAudioNode()));
  }

  Napi::Value AudioPlayer::VolumeGetter(const Napi::CallbackInfo &info)
  {
    return Napi::Number::New(info.Env(), volume);
  }

  void AudioPlayer::VolumeSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (!value.IsNumber())
    {
      Napi::TypeError::New(info.Env(), "Volume should be a number").ThrowAsJavaScriptException();
      return;
    }

    volume = value.As<Napi::Number>().FloatValue();
    if (volume > 1.0f)
      volume = 1.0f;
    else if (volume < 0.0f)
      volume = 0.0f;

    if (gainNode == nullptr || !gainNode->isInitialized())
      return;
    gainNode->gain()->setValue(volume);
  }

  Napi::Value AudioPlayer::LoopGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return Napi::Boolean::New(env, looped);
  }

  void AudioPlayer::LoopSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (value.IsBoolean())
      looped = value.As<Napi::Boolean>().Value();
  }

  Napi::Value AudioPlayer::OnEndedGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return onended.Value();
  }

  void AudioPlayer::OnEndedSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (value.IsFunction())
    {
      onended.Reset(value.As<Napi::Function>());
      onendedTSFN = Napi::ThreadSafeFunction::New(env, onended.Value(), "Audio:OnEnded", 0, 1);
      audioNode->setOnEnded([this]()
                            { this->onendedTSFN.BlockingCall(this, ProcessEndedInMainThread); });
    }
    else
    {
      onended.Reset();
      onendedTSFN.Unref(env);
      onendedTSFN.Release();
    }
  }

  void AudioPlayer::SetupAudioNodeAndBus()
  {
    if (!sourced)
    {
      auto ac = getDefaultAudioContext();
      lab::ContextRenderLock lock(ac, "Audio::ProcessLoadInMainThread 1");
      audioNode->setBus(lock, audioBus);
      gainNode->gain()->setValue(volume);
      ac->connect(gainNode, audioNode, 0, 0);
      ac->connect(ac->device(), gainNode, 0, 0);
    }
    else
    {
      lab::ContextRenderLock lock(sourceAudioContext, "Audio::ProcessLoadInMainThread 2");
      sourceAudioNode->setBus(lock, audioBus);
      gainNode->gain()->setValue(volume);
      sourceAudioContext->connect(gainNode, sourceAudioNode, 0, 0);
      sourceAudioContext->connect(sourceAudioContext->device(), gainNode, 0, 0);
    }
    loaded = true;
  }

  void AudioPlayer::ProcessLoadInMainThread(Napi::Env env, Napi::Function callback, AudioPlayer *self)
  {
    Napi::HandleScope scope(env);

    self->SetupAudioNodeAndBus();
    callback.Call({Napi::String::New(env, self->error)});

    self->audioBus.reset();
    self->error = "";
    self->cbFn.Reset();
    self->onloadedTSFN.Release();

    if (self->loadingThread.joinable())
      self->loadingThread.join();
  }

  void AudioPlayer::ProcessEndedInMainThread(Napi::Env env, Napi::Function callback, AudioPlayer *self)
  {
    Napi::HandleScope scope(env);
    callback.Call({});
    // self->onended.Reset();
    // self->onendedTSFN.Release();
  }

  float GetDurationOnSampledAudioNode(lab::SampledAudioNode *node)
  {
    auto bus = node->getBus();
    if (bus)
      return bus->length() / bus->sampleRate();
    else
      return 0;
  }

  bool isMp3(std::vector<unsigned char> data)
  {
    if (data.size() < 3)
      return false;

    return (data[0] == 0x49 && data[1] == 0x44 && data[2] == 0x33) || // id3
           (data[0] == 0xff && (data[1] & 0xe0) == 0xe0) ||           // no tag
           (data[0] == 0x54 && data[1] == 0x41 && data[2] == 0x47);   // 'TAG'
  }

  bool isWav(std::vector<unsigned char> data)
  {
    if (data.size() < 12)
      return false;

    return data[0] == 82 &&
           data[1] == 73 &&
           data[2] == 70 &&
           data[3] == 70 &&
           data[8] == 87 &&
           data[9] == 65 &&
           data[10] == 86 &&
           data[11] == 69;
  }

  bool isOgg(std::vector<unsigned char> data)
  {
    if (data.size() < 4)
      return false;

    return data[0] == 79 &&
           data[1] == 103 &&
           data[2] == 103 &&
           data[3] == 83;
  }

  SupportedAudioFormat DetectAudioFormat(std::vector<unsigned char> audioBuffer)
  {
    if (isMp3(audioBuffer))
      return MP3;
    else if (isWav(audioBuffer))
      return WAV;
    else if (isOgg(audioBuffer))
      return OGG;
    else
      return UNKNOWN;
  }

  Napi::Object InitWebAudioModule(Napi::Env env, Napi::Object exports)
  {
    AudioContext::Init(env, exports);
    AudioNode::Init(env, exports);
    AudioPlayer::Init(env, exports);
    return exports;
  }

} // namespace webaudio
