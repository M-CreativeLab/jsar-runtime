#include "event_target.hpp"
#include "common/events/event_type.hpp"
#include "common/events/event.hpp"
#include "common/events/message.hpp"

namespace bindings
{
  namespace messaging
  {
    Napi::FunctionReference *NativeEventTarget::constructor;

    void NativeEventTarget::Init(Napi::Env env, Napi::Object exports)
    {
      Napi::Function tpl = DefineClass(
          env,
          "NativeEventTarget",
          {InstanceMethod("dispatchEvent", &NativeEventTarget::DispatchEvent)});

      constructor = new Napi::FunctionReference();
      *constructor = Napi::Persistent(tpl);
      env.SetInstanceData(constructor);

      exports.Set("NativeEventTarget", tpl);
    }

    NativeEventTarget::NativeEventTarget(const Napi::CallbackInfo &info) : Napi::ObjectWrap<NativeEventTarget>(info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      clientContext = TrClientContextPerProcess::Get();
      if (clientContext == nullptr)
      {
        Napi::Error::New(env, "The client context is not initialized.").ThrowAsJavaScriptException();
        return;
      }
      if (info.Length() < 1 || !info[0].IsFunction())
      {
        Napi::Error::New(env, "The first argument should be a function.").ThrowAsJavaScriptException();
        return;
      }

      eventListener = Napi::ThreadSafeFunction::New(
          env,
          info[0].As<Napi::Function>(),
          "NativeEventTarget",
          0,
          1);
    }

    NativeEventTarget::~NativeEventTarget()
    {
      eventListener.Release();
    }

    Napi::Value NativeEventTarget::DispatchEvent(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      if (info.Length() < 1 || !info[0].IsObject())
      {
        Napi::Error::New(env, "The first argument should be an object.").ThrowAsJavaScriptException();
        return env.Undefined();
      }

      Napi::Object jsEventInit = info[0].As<Napi::Object>();
      Napi::Value jsType = jsEventInit.Get("type");
      Napi::Value jsDetail = jsEventInit.Get("detail");
      if (!jsType.IsNumber() || !jsDetail.IsString())
      {
        Napi::Error::New(env, "The event object should have id, type and data properties.").ThrowAsJavaScriptException();
        return env.Undefined();
      }

      auto type = static_cast<TrEventType>(jsType.As<Napi::Number>().Int32Value());
      auto detail = jsDetail.As<Napi::String>().Utf8Value();

      TrEvent event(type, detail);
      TrEventMessage msg(event);
      clientContext->sendEventMessage(msg);
      return Napi::Number::New(env, event.id);
    }
  }
}
