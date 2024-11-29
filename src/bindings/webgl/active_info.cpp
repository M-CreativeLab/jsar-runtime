#include "./active_info.hpp"

namespace webgl
{
  thread_local Napi::FunctionReference *WebGLActiveInfo::constructor;
  void WebGLActiveInfo::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "WebGLActiveInfo", {});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
  }

  Napi::Object WebGLActiveInfo::NewInstance(Napi::Env env, client_graphics::WebGLActiveInfo &activeInfo)
  {
    Napi::EscapableHandleScope scope(env);

    auto external = Napi::External<client_graphics::WebGLActiveInfo>::New(env, new client_graphics::WebGLActiveInfo(activeInfo));
    Napi::Object obj = constructor->New({external});
    return scope.Escape(napi_value(obj)).ToObject();
  }

  WebGLActiveInfo::WebGLActiveInfo(const Napi::CallbackInfo &info) : Napi::ObjectWrap<WebGLActiveInfo>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsExternal())
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }

    Napi::External<client_graphics::WebGLActiveInfo> external = info[0].As<Napi::External<client_graphics::WebGLActiveInfo>>();
    client_graphics::WebGLActiveInfo *activeInfo = external.Data();
    if (activeInfo == nullptr)
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }
    name = activeInfo->name;
    size = activeInfo->size;
    type = activeInfo->type;
    delete activeInfo;

    auto jsThis = info.This().As<Napi::Object>();
    jsThis.Set("name", Napi::String::New(env, name));
    jsThis.Set("size", Napi::Number::New(env, size));
    jsThis.Set("type", Napi::Number::New(env, type));
  }
}
