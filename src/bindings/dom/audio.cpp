#include "./audio.hpp"

namespace bindings
{
  namespace dom
  {
    Napi::FunctionReference *Audio::constructor;
    void Audio::Init(Napi::Env env, Napi::Object exports)
    {
      Napi::Function func = DefineClass(env, "Audio",
                                        {InstanceMethod("canPlayType", &Audio::CanPlayType),
                                         InstanceMethod("fastSeek", &Audio::FastSeek),
                                         InstanceMethod("load", &Audio::Load),
                                         InstanceMethod("pause", &Audio::Pause),
                                         InstanceMethod("play", &Audio::Play),
                                         InstanceAccessor("duration", &Audio::DurationGetter, nullptr),
                                         InstanceAccessor("loop", &Audio::LoopGetter, &Audio::LoopSetter),
                                         InstanceAccessor("volume", &Audio::VolumeGetter, &Audio::VolumeSetter)});

      constructor = new Napi::FunctionReference();
      *constructor = Napi::Persistent(func);
      env.SetInstanceData(constructor);
      exports.Set("Audio", func);
      // exports.Set("HTMLAudioElement", func);
    }

    Audio::Audio(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Audio>(info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      clientContext = TrClientContextPerProcess::Get();
      assert(clientContext != nullptr);

      auto jsThis = info.This().As<Napi::Object>();
      auto fnObject = Napi::Function::New(env, OnMediaEvent).ToObject();
      Napi::Function eventHandlerFn = fnObject
                                          .Get("bind")
                                          .As<Napi::Function>()
                                          .Call(fnObject, {jsThis})
                                          .As<Napi::Function>();
      onMediaEvent = Napi::ThreadSafeFunction::New(env, eventHandlerFn, "MediaEvent", 0, 2);

      handle = clientContext->createAudioPlayer();
      handle->resetGlobalEventListener([this](auto eventType, auto &event)
                                       {
                                        auto eventToJS = new media_client::MediaEvent(event);
                                        onMediaEvent.NonBlockingCall(eventToJS,
                                            [](Napi::Env env, Napi::Function jsCallback, media_client::MediaEvent *event) 
                                            {
                                              if (event == nullptr)
                                                return;
                                              auto jsEventType = Napi::Number::New(env, static_cast<int>(event->type));
                                              jsCallback.Call({jsEventType});
                                              delete event;
                                            }); });
    }

    Audio::~Audio()
    {
      onMediaEvent.Release();
      handle.reset();
    }

    Napi::Value Audio::CanPlayType(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      if (info.Length() < 1 || !info[0].IsString())
      {
        Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
        return env.Null();
      }

      std::string type = info[0].As<Napi::String>().Utf8Value();
      auto result = handle->canPlayType(type);
      if (result == media_client::CanPlayTypeResult::No)
        return Napi::String::New(env, "");
      else if (result == media_client::CanPlayTypeResult::Probably)
        return Napi::String::New(env, "probably");
      else
        return Napi::String::New(env, "maybe");
    }

    Napi::Value Audio::FastSeek(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      if (info.Length() < 1 || !info[0].IsNumber())
      {
        Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
        return env.Null();
      }

      double time = info[0].As<Napi::Number>().DoubleValue();
      handle->fastSeek(time);
      return env.Null();
    }

    Napi::Value Audio::Load(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      auto jsThis = info.This().As<Napi::Object>();
      // TODO: support "src"?
      if (jsThis.Has("srcObject") && jsThis.Get("srcObject").IsObject())
      {
        auto srcObject = jsThis.Get("srcObject").ToObject();
        auto blobConstructorValue = env.Global().Get("Blob").As<Napi::Function>();
        if (srcObject.InstanceOf(blobConstructorValue))
        {
          auto blob = srcObject.ToObject();
          auto arraybuffer = blob.Get("arrayBuffer").As<Napi::Function>().Call(blob, {});
          if (arraybuffer.IsPromise())
          {
            auto thenFunc = arraybuffer.ToObject().Get("then").As<Napi::Function>();
            auto onFulfilled = Napi::Function::New(env, &Audio::OnLoadArrayBuffer);
            auto onFulfilledBond = onFulfilled.Get("bind").As<Napi::Function>().Call(onFulfilled, {jsThis});
            thenFunc.Call(arraybuffer, {onFulfilledBond});
            return env.Undefined();
          }
        }
      }
      Napi::TypeError::New(env, "Invalid arguments").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Value Audio::Pause(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      handle->pause();
      return env.Null();
    }

    Napi::Value Audio::Play(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      handle->play();
      return env.Null();
    }

    Napi::Value Audio::DurationGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      auto duration = handle->getDuration();
      if (duration < 0)
        return env.RunScript("NaN");
      else
        return Napi::Number::New(env, duration);
    }

    void Audio::LoopSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      handle->setLoop(value.ToBoolean().Value());
    }

    Napi::Value Audio::LoopGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      return Napi::Boolean::New(env, handle->getLoop());
    }

    void Audio::VolumeSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      handle->setVolume(value.As<Napi::Number>().DoubleValue());
    }

    Napi::Value Audio::VolumeGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      return Napi::Number::New(env, handle->getVolume());
    }

    Napi::Value Audio::OnLoadArrayBuffer(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      auto jsThis = info.This().As<Napi::Object>();
      auto arraybuffer = info[0].As<Napi::ArrayBuffer>();

      auto audio = Audio::Unwrap(jsThis);
      auto handle = audio->handle.get();
      handle->setSrc(arraybuffer.Data(), arraybuffer.ByteLength());
      handle->load();
      return env.Null();
    }

    Napi::Value Audio::OnMediaEvent(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      auto eventType = info[0].As<Napi::Number>().Int32Value();
      auto eventName = media_comm::eventTypeToEventName(static_cast<media_comm::TrMediaEventType>(eventType));
      if (eventName.empty())
      {
        Napi::TypeError::New(env, "Invalid event type").ThrowAsJavaScriptException();
        return env.Undefined();
      }
      else
      {
        auto jsThis = info.This().As<Napi::Object>();
        auto eventHandler = jsThis.Get(eventName);
        if (eventHandler.IsFunction())
        {
          // TODO: create a Event object
          eventHandler.As<Napi::Function>().Call(jsThis, {});
        }
      }
      return env.Undefined();
    }
  }
}
