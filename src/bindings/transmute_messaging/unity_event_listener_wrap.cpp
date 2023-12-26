#include "unity_event_listener_wrap.hpp"
#include "debug.hpp"

using namespace std;
using namespace messaging;

UnityEventListenerWrap *UnityEventListenerWrap::instance_;
Napi::FunctionReference *UnityEventListenerWrap::constructor;

void UnityEventListenerWrap::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function tpl = DefineClass(
        env,
        "UnityEventListener",
        {InstanceMethod("setCallback", &UnityEventListenerWrap::SetCallback),
         InstanceMethod("dispose", &UnityEventListenerWrap::Dispose)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);

    exports.Set("UnityEventListener", tpl);
}

UnityEventListenerWrap *UnityEventListenerWrap::GetInstance()
{
    if (instance_ == nullptr)
        return nullptr;
    else
        return instance_;
}

UnityEventListenerWrap::UnityEventListenerWrap(const Napi::CallbackInfo &info) : Napi::ObjectWrap<UnityEventListenerWrap>(info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    instance_ = this;
    disposed_ = false;
}

bool UnityEventListenerWrap::Dispatch(string type, string data)
{
    if (disposed_)
        return true;

    JSEventMessage *message = new JSEventMessage();
    message->type = type;
    message->data = data;

    auto callback = [](Napi::Env env, Napi::Function jsCallback, JSEventMessage *message)
    {
        try
        {
            Napi::String typeParam = Napi::String::New(env, message->type);
            Napi::String dataParam = Napi::String::New(env, message->data);
            jsCallback.Call({typeParam, dataParam});
        }
        catch (const Napi::Error &e)
        {
            DEBUG("transmute", "UnityEventListenerWrap::Dispatch() failed to call JS callback: %s", e.Message().c_str());
        }
        catch (...)
        {
            DEBUG("transmute", "UnityEventListenerWrap::Dispatch() failed to call JS callback: Unkown exception.");
        }
        delete message;
    };

    napi_status status = tsfn_.BlockingCall(message, callback);
    if (status != napi_ok)
        fprintf(stderr, "UnityEventListenerWrap::Dispatch() failed to call JS callback\n");

    return true;
}

bool UnityEventListenerWrap::DispatchInputEvent(string data)
{
    return Dispatch("input", data);
}

void UnityEventListenerWrap::Finalize(Napi::Env env)
{
    if (disposed_)
        return;

    tsfn_.Unref(env);
    tsfn_.Release();
    instance_ = nullptr;
    disposed_ = true;
}

Napi::Value UnityEventListenerWrap::SetCallback(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (disposed_)
    {
        Napi::TypeError::New(env, "UnityEventListener is already disposed").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    tsfn_ = Napi::ThreadSafeFunction::New(
        env,
        info[0].As<Napi::Function>(),
        "UnityEventListener",
        0,
        1,
        [](Napi::Env env)
        {
            // TODO: remove the logs
            fprintf(stderr, "UnityEventListenerWrap::SetCallback() finalizer is called\n");
        });

    return info.This();
}

Napi::Value UnityEventListenerWrap::Dispose(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    tsfn_.Unref(env);
    tsfn_.Release();
    instance_ = nullptr;
    disposed_ = true;
    return info.This();
}
