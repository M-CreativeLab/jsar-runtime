#pragma once

#include "./html_media_element.hpp"
#include "./html_element-inl.hpp"

namespace dombinding
{
  template <typename ObjectType, typename HTMLMediaElementType>
  vector<Napi::ClassPropertyDescriptor<ObjectType>> HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::GetClassProperties(Napi::Env env)
  {
    using T = HTMLMediaElementBase<ObjectType, HTMLMediaElementType>;
    auto props = HTMLElementBase<ObjectType, HTMLMediaElementType>::GetClassProperties(env);
    auto added = vector<Napi::ClassPropertyDescriptor<ObjectType>>(
      {
        T::InstanceAccessor("autoplay", &T::AutoPlayGetter, &T::AutoPlaySetter),
        T::InstanceAccessor("currentSrc", &T::CurrentSrcGetter, nullptr),
        T::InstanceAccessor("currentTime", &T::CurrentTimeGetter, &T::CurrentTimeSetter),
        T::InstanceAccessor("duration", &T::DurationGetter, nullptr),
        T::InstanceAccessor("ended", &T::EndedGetter, nullptr),
        T::InstanceAccessor("loop", &T::LoopGetter, &T::LoopSetter),
        T::InstanceAccessor("src", &T::SrcGetter, &T::SrcSetter),
        T::InstanceAccessor("srcObject", &T::SrcObjectGetter, &T::SrcObjectSetter),
        T::InstanceAccessor("volume", &T::VolumeGetter, &T::VolumeSetter),
        T::InstanceMethod("canPlayType", &T::CanPlayType),
        T::InstanceMethod("captureStream", &T::CaptureStream),
        T::InstanceMethod("fastSeek", &T::FastSeek),
        T::InstanceMethod("load", &T::Load),
        T::InstanceMethod("pause", &T::Pause),
        T::InstanceMethod("play", &T::Play),
      });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::HTMLMediaElementBase(const Napi::CallbackInfo &info)
      : HTMLElementBase<ObjectType, HTMLMediaElementType>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  Napi::Value HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::AutoPlayGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, this->node->autoPlay);
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  void HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::AutoPlaySetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    this->node->autoPlay = value.ToBoolean();
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  Napi::Value HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::CurrentSrcGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, this->node->currentSrc());
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  Napi::Value HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::CurrentTimeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, this->node->currentTime);
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  void HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::CurrentTimeSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    this->node->currentTime = value.ToNumber();
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  Napi::Value HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::DurationGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, this->node->duration());
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  Napi::Value HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::EndedGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, this->node->ended());
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  Napi::Value HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::LoopGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, this->node->getLoop());
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  void HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::LoopSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    this->node->setLoop(value.ToBoolean());
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  Napi::Value HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::SrcGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, this->node->getSrc());
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  void HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::SrcSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    this->node->setSrc(value.ToString());
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  Napi::Value HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::SrcObjectGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (srcObjectRef.IsEmpty())
      return env.Undefined();
    else
      return srcObjectRef.Value();
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  void HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::SrcObjectSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (!value.IsObject())
    {
      auto msg =
        "Failed to set the 'srcObject' property on 'HTMLMediaElement': "
        "The provided value is not of object.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return;
    }

    auto global = env.Global();
    auto valueObject = value.ToObject();

    if (valueObject.InstanceOf(global.Get("Blob").As<Napi::Function>()))
    {
      auto arraybuffer = valueObject.Get("arrayBuffer").As<Napi::Function>().Call(valueObject, {});
      if (arraybuffer.IsPromise())
      {
        Napi::Function func = Napi::Function::New(
          env,
          [](const Napi::CallbackInfo &info) -> Napi::Value
          {
            Napi::Env env = info.Env();
            Napi::HandleScope scope(env);

            auto jsThis = info.This().As<Napi::Object>();
            auto arraybuffer = info[0].As<Napi::ArrayBuffer>();
            auto *element = HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::Unwrap(jsThis);
            element->node->setSrcBuffer(arraybuffer.Data(), arraybuffer.ByteLength());
            return env.Undefined();
          });

        auto onArrayBufferLoaded = func.Get("bind").As<Napi::Function>().Call(func, {info.This()});
        arraybuffer
          .ToObject()
          .Get("then")
          .As<Napi::Function>()
          .Call(arraybuffer, {onArrayBufferLoaded});
      }
    }
    /**
     * TODO: Support MediaStream, MediaSource, File
     */
    else
    {
      auto msg =
        "Failed to set the 'srcObject' property on 'HTMLMediaElement': "
        "The provided value is not of type 'MediaStream', 'MediaSource', 'Blob' or 'File'.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return;
    }
    srcObjectRef.Reset(valueObject);
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  Napi::Value HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::VolumeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, this->node->getVolume());
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  void HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::VolumeSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto vol = value.ToNumber().FloatValue();
    this->node->setVolume(vol);
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  Napi::Value HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::CanPlayType(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      auto msg =
        "Failed to execute 'canPlayType' on 'HTMLMediaElement': "
        "1 argument required, but only 0 present.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto mimeTypeStr = info[0].ToString().Utf8Value();
    media_comm::CanPlayTypeResult r = this->node->canPlayType(mimeTypeStr);
    if (r == media_comm::CanPlayTypeResult::Probably)
      return Napi::String::New(env, "probably");
    else if (r == media_comm::CanPlayTypeResult::Maybe)
      return Napi::String::New(env, "maybe");
    else
      return Napi::String::New(env, "");
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  Napi::Value HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::CaptureStream(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Value();
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  Napi::Value HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::FastSeek(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    this->node->fastSeek(info[0].ToNumber().FloatValue());
    return env.Undefined();
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  Napi::Value HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::Load(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    this->node->startLoading();
    return env.Undefined();
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  Napi::Value HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::Pause(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    this->node->pause();
    return env.Undefined();
  }

  template <typename ObjectType, typename HTMLMediaElementType>
  Napi::Value HTMLMediaElementBase<ObjectType, HTMLMediaElementType>::Play(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    this->node->play();
    return env.Undefined();
  }
}
