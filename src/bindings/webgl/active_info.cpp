#include "./active_info.hpp"

namespace webgl
{
  Napi::FunctionReference *WebGLActiveInfo::constructor;

  void WebGLActiveInfo::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);

    Napi::Function func = DefineClass(env, "WebGLActiveInfo", {});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
  }

  Napi::Object WebGLActiveInfo::NewInstance(Napi::Env env, commandbuffers::ActiveInfo activeInfo)
  {
    Napi::EscapableHandleScope scope(env);

    auto external = Napi::External<commandbuffers::ActiveInfo>::New(env, new commandbuffers::ActiveInfo(activeInfo));
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

    Napi::External<commandbuffers::ActiveInfo> external = info[0].As<Napi::External<commandbuffers::ActiveInfo>>();
    commandbuffers::ActiveInfo *activeInfo = external.Data();
    if (activeInfo == nullptr)
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }
    this->name = activeInfo->name;
    this->size = activeInfo->size;
    this->type = activeInfo->type;
    delete activeInfo;

    auto jsThis = info.This().As<Napi::Object>();
    jsThis.Set("name", Napi::String::New(env, this->name));
    jsThis.Set("size", Napi::Number::New(env, this->size));
    jsThis.Set("type", Napi::Number::New(env, this->type));
  }
}
