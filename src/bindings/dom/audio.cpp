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
                                         InstanceMethod("play", &Audio::Play)});

      constructor = new Napi::FunctionReference();
      *constructor = Napi::Persistent(func);
      env.SetInstanceData(constructor);
      exports.Set("Audio", func);
    }

    Audio::Audio(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Audio>(info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      handle = std::make_unique<media::AudioPlayer>();
    }

    Audio::~Audio()
    {
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
      bool result = handle->canPlayType(type);
      return Napi::Boolean::New(env, result);
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
  }
}
