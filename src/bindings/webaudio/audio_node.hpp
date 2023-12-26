#pragma once

#include <vector>
#include <napi.h>
#include <LabSound/LabSound.h>

using namespace std;

namespace webaudio
{
  class AudioNode : public Napi::ObjectWrap<AudioNode>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    AudioNode(const Napi::CallbackInfo &info);

  private:
    static Napi::FunctionReference *constructor;

    Napi::Value Connect(const Napi::CallbackInfo &info);
    Napi::Value Disconnect(const Napi::CallbackInfo &info);
    Napi::Value ContextGetter(const Napi::CallbackInfo &info);
    Napi::Value NumberOfInputsGetter(const Napi::CallbackInfo &info);
    Napi::Value NumberOfOutputsGetter(const Napi::CallbackInfo &info);
    Napi::Value ChannelCountGetter(const Napi::CallbackInfo &info);
    Napi::Value ChannelCountModeGetter(const Napi::CallbackInfo &info);
    Napi::Value ChannelInterpretationGetter(const Napi::CallbackInfo &info);

    Napi::Reference<Napi::Object> context;
    std::shared_ptr<lab::AudioNode> audioNode;
    Napi::Reference<Napi::Array> inputAudioNodes;
    Napi::Reference<Napi::Array> outputAudioNodes;
  };

} // namespace webaudio
