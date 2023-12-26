#include <napi.h>
#include "audio_node.hpp"
#include "audio_context.hpp"

namespace webaudio
{
  Napi::FunctionReference *AudioNode::constructor;
  void AudioNode::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::HandleScope scope(env);
    Napi::Function tpl = DefineClass(env, "AudioNode", {
                                                           InstanceMethod("connect", &AudioNode::Connect),
                                                           InstanceMethod("disconnect", &AudioNode::Disconnect),
                                                           InstanceAccessor("context", &AudioNode::ContextGetter, nullptr),
                                                           InstanceAccessor("numberOfInputs", &AudioNode::NumberOfInputsGetter, nullptr),
                                                           InstanceAccessor("numberOfOutputs", &AudioNode::NumberOfOutputsGetter, nullptr),
                                                           InstanceAccessor("channelCount", &AudioNode::ChannelCountGetter, nullptr),
                                                           InstanceAccessor("channelCountMode", &AudioNode::ChannelCountModeGetter, nullptr),
                                                           InstanceAccessor("channelInterpretation", &AudioNode::ChannelInterpretationGetter, nullptr),
                                                       });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);

    exports.Set("AudioNode", tpl);
  }

  AudioNode::AudioNode(const Napi::CallbackInfo &info) : Napi::ObjectWrap<AudioNode>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    inputAudioNodes = Napi::Reference<Napi::Array>::New(Napi::Array::New(env, 0));
    outputAudioNodes = Napi::Reference<Napi::Array>::New(Napi::Array::New(env, 0));
  }

  Napi::Value AudioNode::Connect(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "AudioNode::Connect: expected 1 argument").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsObject())
    {
      Napi::TypeError::New(env, "AudioNode::Connect: expected argument 1 to be an object").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Object constructor = info[0].As<Napi::Object>();
    Napi::String constructorName = constructor.Get("constructor").As<Napi::Object>().Get("name").As<Napi::String>();

    if (
        constructorName.StrictEquals(Napi::String::New(env, "AudioSourceNode")) ||
        constructorName.StrictEquals(Napi::String::New(env, "AudioDestinationNode")) ||
        constructorName.StrictEquals(Napi::String::New(env, "GainNode")) ||
        constructorName.StrictEquals(Napi::String::New(env, "AnalyserNode")) ||
        constructorName.StrictEquals(Napi::String::New(env, "PannerNode")) ||
        constructorName.StrictEquals(Napi::String::New(env, "StereoPannerNode")) ||
        constructorName.StrictEquals(Napi::String::New(env, "ScriptProcessorNode")))
    {
      unsigned int outputIndex = info[1].IsNumber() ? info[1].As<Napi::Number>().Uint32Value() : 0;
      unsigned int inputIndex = info[2].IsNumber() ? info[2].As<Napi::Number>().Uint32Value() : 0;

      Napi::Object srcAudioNodeObj = info.This().As<Napi::Object>();
      AudioNode *srcAudioNode = Napi::ObjectWrap<AudioNode>::Unwrap(srcAudioNodeObj);
      std::shared_ptr<lab::AudioNode> srcLabAudioNode = srcAudioNode->audioNode;

      Napi::Object dstAudioNodeObj = info[0].As<Napi::Object>();
      AudioNode *dstAudioNode = Napi::ObjectWrap<AudioNode>::Unwrap(dstAudioNodeObj);
      std::shared_ptr<lab::AudioNode> dstLabAudioNode = dstAudioNode->audioNode;

      Napi::Object audioContextObj = srcAudioNode->context.Value();
      AudioContext *audioContext = Napi::ObjectWrap<AudioContext>::Unwrap(audioContextObj);

      try
      {
        audioContext->audioContext->connect(dstLabAudioNode, srcLabAudioNode, outputIndex, inputIndex);
      }
      catch (const std::exception &e)
      {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Undefined();
      }
      catch (...)
      {
        Napi::Error::New(env, "unknown exception").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      srcAudioNode->outputAudioNodes.Value().Set(outputIndex, dstAudioNodeObj);
      dstAudioNode->inputAudioNodes.Value().Set(inputIndex, srcAudioNodeObj);
      return info[0];
    }
    else
    {
      Napi::Error::New(env, "AudioNode::Connect: invalid arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    return env.Undefined();
  }

  Napi::Value AudioNode::Disconnect(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();

    if (info.Length() == 0)
    {
      Napi::Object srcAudioNodeObj = info.This().As<Napi::Object>();
      AudioNode *srcAudioNode = Napi::ObjectWrap<AudioNode>::Unwrap(srcAudioNodeObj);
      std::shared_ptr<lab::AudioNode> srcLabAudioNode = srcAudioNode->audioNode;

      Napi::Object audioContextObj = srcAudioNode->context.Value();
      AudioContext *audioContext = Napi::ObjectWrap<AudioContext>::Unwrap(audioContextObj);
      audioContext->audioContext->disconnect(nullptr, srcLabAudioNode);

      Napi::Array outputAudioNodes = srcAudioNode->outputAudioNodes.Value();
      Napi::Array inputAudioNodes = srcAudioNode->inputAudioNodes.Value();
      size_t numOutputAudioNodes = outputAudioNodes.Length();
      size_t numInputAudioNodes = inputAudioNodes.Length();
      for (size_t i = 0; i < numOutputAudioNodes; i++)
      {
        Napi::Value outputAudioNode = outputAudioNodes.Get(i);

        if (outputAudioNode.IsObject())
        {
          // Napi::Object outputAudioNodeObj = outputAudioNode.As<Napi::Object>();
          // AudioNode *outputAudioNode = Napi::ObjectWrap<AudioNode>::Unwrap(outputAudioNodeObj);

          for (size_t j = 0; j < numInputAudioNodes; j++)
          {
            Napi::Value inputAudioNode = inputAudioNodes.Get(j);
            if (inputAudioNode.IsObject())
            {
              Napi::Object inputAudioNodeObj = inputAudioNode.As<Napi::Object>();
              AudioNode *inputAudioNode = Napi::ObjectWrap<AudioNode>::Unwrap(inputAudioNodeObj);
              if (inputAudioNode == srcAudioNode)
                inputAudioNodes.Set(j, env.Null());
            }
          }
          outputAudioNodes.Set(i, env.Null());
        }
      }
    }
    else
    {
      if (info[0].IsObject())
      {
        Napi::Value constructorName = info[0].As<Napi::Object>().Get("constructor").As<Napi::Object>().Get("name");

        if (
            constructorName.StrictEquals(Napi::String::New(env, "AudioSourceNode")) ||
            constructorName.StrictEquals(Napi::String::New(env, "AudioDestinationNode")) ||
            constructorName.StrictEquals(Napi::String::New(env, "GainNode")) ||
            constructorName.StrictEquals(Napi::String::New(env, "AnalyserNode")) ||
            constructorName.StrictEquals(Napi::String::New(env, "PannerNode")) ||
            constructorName.StrictEquals(Napi::String::New(env, "StereoPannerNode")) ||
            constructorName.StrictEquals(Napi::String::New(env, "ScriptProcessorNode")))
        {
          Napi::Object srcAudioNodeObj = info.This().As<Napi::Object>();
          AudioNode *srcAudioNode = Napi::ObjectWrap<AudioNode>::Unwrap(srcAudioNodeObj);
          std::shared_ptr<lab::AudioNode> srcLabAudioNode = srcAudioNode->audioNode;

          Napi::Object dstAudioNodeObj = info[0].As<Napi::Object>();
          AudioNode *dstAudioNode = Napi::ObjectWrap<AudioNode>::Unwrap(dstAudioNodeObj);
          std::shared_ptr<lab::AudioNode> dstLabAudioNode = dstAudioNode->audioNode;

          Napi::Object audioContextObj = srcAudioNode->context.Value();
          AudioContext *audioContext = Napi::ObjectWrap<AudioContext>::Unwrap(audioContextObj);

          audioContext->audioContext->disconnect(dstLabAudioNode, srcLabAudioNode);

          Napi::Array outputAudioNodes = srcAudioNode->outputAudioNodes.Value();
          Napi::Array inputAudioNodes = srcAudioNode->inputAudioNodes.Value();
          size_t numOutputAudioNodes = outputAudioNodes.Length();
          size_t numInputAudioNodes = inputAudioNodes.Length();
          for (size_t i = 0; i < numOutputAudioNodes; i++)
          {
            Napi::Value outputAudioNode = outputAudioNodes.Get(i);

            if (outputAudioNode.IsObject())
            {
              Napi::Object outputAudioNodeObj = outputAudioNode.As<Napi::Object>();
              AudioNode *outputAudioNode = Napi::ObjectWrap<AudioNode>::Unwrap(outputAudioNodeObj);

              if (outputAudioNode == dstAudioNode)
              {
                for (size_t j = 0; j < numInputAudioNodes; j++)
                {
                  Napi::Value inputAudioNode = inputAudioNodes.Get(j);

                  if (inputAudioNode.IsObject())
                  {
                    Napi::Object inputAudioNodeObj = inputAudioNode.As<Napi::Object>();
                    AudioNode *inputAudioNode = Napi::ObjectWrap<AudioNode>::Unwrap(inputAudioNodeObj);

                    if (inputAudioNode == srcAudioNode)
                    {
                      inputAudioNodes.Set(j, env.Null());
                    }
                  }
                }
                outputAudioNodes.Set(i, env.Null());
              }
            }
          }
          return info[0];
        }
        else
        {
          Napi::Error::New(env, "AudioNode::Disconnect: invalid arguments").ThrowAsJavaScriptException();
        }
      }
      else
      {
        Napi::Error::New(env, "AudioNode::Disconnect: invalid arguments").ThrowAsJavaScriptException();
      }
    }
    return env.Null();
  }

  Napi::Value AudioNode::ContextGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
  }

  Napi::Value AudioNode::NumberOfInputsGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, audioNode->numberOfInputs());
  }

  Napi::Value AudioNode::NumberOfOutputsGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, audioNode->numberOfOutputs());
  }

  Napi::Value AudioNode::ChannelCountGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, audioNode->channelCount());
  }

  Napi::Value AudioNode::ChannelCountModeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::String result;
    auto channelCountMode = audioNode->channelCountMode();

    switch (channelCountMode)
    {
    case lab::ChannelCountMode::Max:
      result = Napi::String::New(env, "max");
      break;
    case lab::ChannelCountMode::ClampedMax:
      result = Napi::String::New(env, "clamped-max");
      break;
    case lab::ChannelCountMode::Explicit:
      result = Napi::String::New(env, "explicit");
      break;
    default:
      result = Napi::String::New(env, "");
      break;
    }
    return result;
  }

  Napi::Value AudioNode::ChannelInterpretationGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::String result;
    auto channelInterpretation = audioNode->channelInterpretation();

    switch (channelInterpretation)
    {
    case lab::ChannelInterpretation::Speakers:
      result = Napi::String::New(env, "speakers");
      break;
    case lab::ChannelInterpretation::Discrete:
      result = Napi::String::New(env, "discrete");
      break;
    default:
      result = Napi::String::New(env, "");
      break;
    }
    return result;
  }
}