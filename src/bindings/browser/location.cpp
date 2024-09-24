#include "common/utility.hpp"
#include "./location.hpp"

namespace browserbinding
{
  Napi::FunctionReference *Location::constructor;
  void Location::Init(Napi::Env env)
  {
    Napi::Function func = DefineClass(env, "Location", {
      InstanceMethod("assign", &Location::assign),
      InstanceMethod("reload", &Location::reload),
      InstanceMethod("replace", &Location::replace),
      InstanceMethod("toString", &Location::toString),
    });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    env.Global().Set("Location", func);
  }

  Napi::Object Location::NewInstance(Napi::Env env, std::string url)
  {
    Napi::EscapableHandleScope scope(env);
    /**
     * TODO: Implement the following code.
     */
  }

  Location::Location(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Location>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 && info[0].IsExternal())
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }
    locationImpl = *info[0].As<Napi::External<browser::Location>>().Data();

    auto jsThis = info.This().As<Napi::Object>();
    jsThis.Set("hash", Napi::String::New(env, locationImpl.hash));
    jsThis.Set("host", Napi::String::New(env, locationImpl.host));
    jsThis.Set("hostname", Napi::String::New(env, locationImpl.hostname));
    jsThis.Set("href", Napi::String::New(env, locationImpl.href));
    jsThis.Set("origin", Napi::String::New(env, locationImpl.origin));
    jsThis.Set("pathname", Napi::String::New(env, locationImpl.pathname));
    jsThis.Set("port", Napi::Number::New(env, locationImpl.port));
    jsThis.Set("protocol", Napi::String::New(env, locationImpl.protocol));
  }
}
