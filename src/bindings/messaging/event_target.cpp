#include "./event_target.hpp"

namespace bindings
{
  namespace messaging
  {
    Napi::FunctionReference *NativeEventTarget::constructor;

    void NativeEventTarget::Init(Napi::Env env, Napi::Object exports)
    {
      Napi::HandleScope scope(env);

      // `EventTypes`
      Napi::Object jsEventTypes = Napi::Object::New(env);
#define XX(type) \
  jsEventTypes.Set(#type, Napi::Number::New(env, static_cast<int>(TrNativeEventType::type)));
      TR_NATIVE_EVENTS_MAP(XX)
#undef XX

      // `DocumentEventTypes`
      Napi::Object jsDocumentEventTypes = Napi::Object::New(env);
#define XX(type, jsName) \
  jsDocumentEventTypes.Set(#jsName, Napi::Number::New(env, static_cast<int>(TrDocumentEventType::type)));
      TR_DOCUMENT_EVENTS_MAP(XX)
#undef XX

      Napi::Function tpl = DefineClass(
          env,
          "NativeEventTarget",
          {
              StaticValue("EventTypes", jsEventTypes),
              StaticValue("DocumentEventTypes", jsDocumentEventTypes),
              InstanceMethod("dispatchEvent", &NativeEventTarget::DispatchEvent),
          });
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

      recvingEvents = true;
      eventRecvThread = thread([this]()
                               {
        SET_THREAD_NAME("TrEventRecv");
        while (recvingEvents)
        {
          TrNativeEventMessage* newEventMessage = clientContext->recvEventMessage(-1);
          if (newEventMessage == nullptr)
            continue;

          eventListener.NonBlockingCall(newEventMessage, [](Napi::Env env, Napi::Function jsCallback, TrNativeEventMessage *eventMessage) {
            switch (eventMessage->getType())
            {
#define CASE_EVENT_TYPE(eventType)                                                                 \
  case TrNativeEventType::eventType:                                                               \
  {                                                                                                \
    auto sharedEvent = TrSharedNativeEventBase::FromMessage<Tr##eventType##Remote>(*eventMessage); \
    auto jsEventId = Napi::Number::New(env, sharedEvent.eventId);                                  \
    auto jsType = Napi::Number::New(env, static_cast<int>(sharedEvent.type));                      \
    auto jsPeerId = Napi::Number::New(env, sharedEvent.peerId);                                    \
    auto jsDetail = Napi::String::New(env, sharedEvent.detailJson);                                \
    jsCallback.Call({jsEventId, jsType, jsPeerId, jsDetail});                                      \
    break;                                                                                         \
  }
              CASE_EVENT_TYPE(RpcRequest)
              CASE_EVENT_TYPE(RpcResponse)
              CASE_EVENT_TYPE(DocumentRequest)
#undef CASE_EVENT_TYPE
              default:
                fprintf(stderr, "Unknown event type: %d\n", eventMessage->getType());
                break;
            }
            delete eventMessage;
          });
        } });
    }

    NativeEventTarget::~NativeEventTarget()
    {
      recvingEvents = false;
      eventRecvThread.join();
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

      auto eventType = static_cast<TrNativeEventType>(jsType.As<Napi::Number>().Int32Value());
      auto detailJsonSrc = jsDetail.As<Napi::String>().Utf8Value();
      if (
          eventType != TrNativeEventType::RpcRequest &&
          eventType != TrNativeEventType::RpcResponse &&
          eventType != TrNativeEventType::DocumentEvent)
      {
        string msg = "The event type is not supported: " + to_string(static_cast<int>(eventType));
        Napi::Error::New(env, msg).ThrowAsJavaScriptException();
        return env.Undefined();
      }

      TrNativeEvent event = TrNativeEvent::MakeEventWithString(eventType, detailJsonSrc.c_str());
      clientContext->sendEvent(event);
      return Napi::Number::New(env, event.id);
    }
  }
}
