#include "common/utility.hpp"
#include "./location.hpp"

namespace browserbinding
{
  thread_local Napi::FunctionReference *Location::constructor;
  void Location::Init(Napi::Env env)
  {
    Napi::Function func = DefineClass(env, "Location",
                                      {
                                          InstanceMethod("assign", &Location::Assign, napi_property_attributes::napi_default_jsproperty),
                                          InstanceMethod("reload", &Location::Reload, napi_property_attributes::napi_default_jsproperty),
                                          InstanceMethod("replace", &Location::Replace, napi_property_attributes::napi_default_jsproperty),
                                          InstanceMethod("toString", &Location::ToString, napi_property_attributes::napi_default_jsproperty),
                                      });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("Location", func);
  }

  Napi::Object Location::NewInstance(Napi::Env env, std::string url)
  {
    Napi::EscapableHandleScope scope(env);
    SharedReference locationRef(std::make_shared<browser::Location>(url));
    auto external = Napi::External<SharedReference<browser::Location>>::New(env, &locationRef);
    auto instance = constructor->New({external});
    return scope.Escape(instance).ToObject();
  }

  Location::Location(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<Location>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 && info[0].IsExternal())
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }

    SharedReference<browser::Location> locationRef = *info[0].As<Napi::External<SharedReference<browser::Location>>>().Data();
    locationImpl = *locationRef.value.get();

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

  Napi::Value Location::Assign(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsString())
    {
      Napi::TypeError::New(env, "Invalid argument").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::string url = info[0].As<Napi::String>().Utf8Value();
    return env.Undefined();
  }

  Napi::Value Location::Reload(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsBoolean())
    {
      Napi::TypeError::New(env, "Invalid argument").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    bool forceReload = info[0].As<Napi::Boolean>().Value();
    return env.Undefined();
  }

  Napi::Value Location::Replace(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsString())
    {
      Napi::TypeError::New(env, "Invalid argument").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::string url = info[0].As<Napi::String>().Utf8Value();
    return env.Undefined();
  }

  Napi::Value Location::ToString(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return Napi::String::New(env, locationImpl.href);
  }
}
