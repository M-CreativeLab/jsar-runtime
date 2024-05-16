#include "client_context.hpp"

namespace bindings
{
  Napi::FunctionReference *ClientContext::constructor;

  void ClientContext::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function func = DefineClass(env, "ClientContext", {});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("ClientContext", func);
  }

  ClientContext::ClientContext(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ClientContext>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    clientContext = TrClientContextPerProcess::Get();
    if (clientContext == nullptr)
    {
      Napi::TypeError::New(env, "Client context is not available.").ThrowAsJavaScriptException();
      return;
    }

    auto thisObject = info.This().ToObject();
    thisObject.Set("id", Napi::Number::New(env, clientContext->id));
    thisObject.Set("url", Napi::String::New(env, clientContext->url));
    thisObject.Set("applicationCacheDirectory",
                   Napi::String::New(env, clientContext->applicationCacheDirectory));
    thisObject.Set("httpsProxyServer", Napi::String::New(env, clientContext->httpsProxyServer));
  }
}
