#include "input_source.hpp"

namespace bindings
{
  Napi::FunctionReference *XRInputSource::constructor;
  Napi::FunctionReference *XRInputSourceArray::constructor;

  Napi::Object XRInputSource::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function func = DefineClass(env, "XRInputSource", {
                                                                InstanceAccessor("gamepad", &XRInputSource::GamepadGetter, nullptr),
                                                                InstanceAccessor("gripSpace", &XRInputSource::GripSpaceGetter, nullptr),
                                                                InstanceAccessor("hand", &XRInputSource::HandGetter, nullptr),
                                                                InstanceAccessor("handness", &XRInputSource::HandnessGetter, nullptr),
                                                                InstanceAccessor("targetRayMode", &XRInputSource::TargetRayModeGetter, nullptr),
                                                                InstanceAccessor("targetRaySpace", &XRInputSource::TargetRaySpaceGetter, nullptr),
                                                            });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("XRInputSource", func);
    return exports;
  }

  Napi::Object XRInputSource::NewInstance(Napi::Env env)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor->New({});
    return scope.Escape(obj).ToObject();
  }

  XRInputSource::XRInputSource(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRInputSource>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
  }

  XRInputSource::~XRInputSource()
  {
  }

  Napi::Value XRInputSource::GamepadGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSource::GripSpaceGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSource::HandGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSource::HandnessGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSource::TargetRayModeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSource::TargetRaySpaceGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Object XRInputSourceArray::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function func = DefineClass(env, "XRInputSourceArray", {
                                                                     InstanceAccessor("length", &XRInputSourceArray::LengthGetter, nullptr),
                                                                     InstanceAccessor("index", &XRInputSourceArray::IndexGetter, nullptr),
                                                                     InstanceMethod("entries", &XRInputSourceArray::Entries),
                                                                     InstanceMethod("forEach", &XRInputSourceArray::ForEach),
                                                                     InstanceMethod("keys", &XRInputSourceArray::Keys),
                                                                     InstanceMethod("values", &XRInputSourceArray::Values),
                                                                 });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("XRInputSourceArray", func);
    return exports;
  }

  Napi::Object XRInputSourceArray::NewInstance(Napi::Env env)
  {
    Napi::EscapableHandleScope scope(env);
    Napi::Object obj = constructor->New({});
    return scope.Escape(obj).ToObject();
  }

  XRInputSourceArray::XRInputSourceArray(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRInputSourceArray>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    inputSources = Napi::Persistent(Napi::Array::New(env));
  }

  XRInputSourceArray::~XRInputSourceArray()
  {
    inputSources.Unref();
  }

  Napi::Value XRInputSourceArray::LengthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSourceArray::IndexGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSourceArray::Entries(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSourceArray::ForEach(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSourceArray::Keys(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSourceArray::Values(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }
}
