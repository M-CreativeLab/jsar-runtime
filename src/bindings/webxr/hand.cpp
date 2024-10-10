#include "hand.hpp"
#include "space.hpp"

namespace bindings
{
  thread_local Napi::FunctionReference *XRHand::constructor;
  Napi::Object XRHand::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRHand",
                                     {InstanceMethod("entries", &XRHand::Entries),
                                      InstanceMethod("forEach", &XRHand::ForEach),
                                      InstanceMethod("get", &XRHand::Get),
                                      InstanceMethod("keys", &XRHand::Keys),
                                      InstanceMethod("values", &XRHand::Values)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);
    exports.Set("XRHand", tpl);
    return exports;
  }

  Napi::Object XRHand::NewInstance(Napi::Env env, xr::TrXRInputSource *inputSourceInternal)
  {
    Napi::EscapableHandleScope scope(env);
    auto inputSourceExternal = Napi::External<xr::TrXRInputSource>::New(env, inputSourceInternal);
    Napi::Object obj = constructor->New({inputSourceExternal});
    return scope.Escape(obj).ToObject();
  }

  XRHand::XRHand(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRHand>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1)
    {
      Napi::TypeError::New(env, "XRHand constructor expects 1 argument").ThrowAsJavaScriptException();
      return;
    }
    if (!info[0].IsExternal())
    {
      Napi::TypeError::New(env, "XRHand constructor could not be called").ThrowAsJavaScriptException();
      return;
    }

    auto inputSourceExternal = info[0].As<Napi::External<xr::TrXRInputSource>>();
    internal = inputSourceExternal.Data();

    for (auto joint : internal->joints)
    {
      auto jointSpaceObject = XRJointSpace::NewInstance(env, internal, joint.index);
      auto nameStr = jointSpaceObject.Get("jointName").ToString().Utf8Value();
      auto jointSpaceObjectRef = new Napi::ObjectReference(Napi::Persistent(jointSpaceObject));
      entries.insert(std::pair<std::string, Napi::ObjectReference *>(nameStr, jointSpaceObjectRef));
    }
  }

  XRHand::~XRHand()
  {
    for (auto &entry : entries)
      delete entry.second;
    entries.clear();
  }

  Napi::Value XRHand::Entries(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto entriesObject = Napi::Object::New(env);
    for (auto &entry : entries)
      entriesObject.Set(entry.first, entry.second->Value());
    return entriesObject;
  }

  Napi::Value XRHand::ForEach(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto callback = info[0].As<Napi::Function>();
    for (auto &entry : entries)
    {
      Napi::Value key = Napi::String::New(env, entry.first);
      Napi::Value value = entry.second->Value();
      callback.Call({value, key, info.This()});
    }
    return env.Null();
  }

  Napi::Value XRHand::Get(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1)
    {
      Napi::TypeError::New(env, "XRHand.get expects 1 argument").ThrowAsJavaScriptException();
      return env.Null();
    }

    auto key = info[0].ToString().Utf8Value();
    auto entry = entries.find(key);
    if (entry == entries.end())
      return env.Null();
    return entry->second->Value();
  }

  Napi::Value XRHand::Keys(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto keysArray = Napi::Array::New(env, entries.size());
    int i = 0;
    for (auto &entry : entries)
      keysArray[i++] = Napi::String::New(env, entry.first);
    return keysArray;
  }

  Napi::Value XRHand::Values(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    
    auto valuesArray = Napi::Array::New(env, entries.size());
    int i = 0;
    for (auto &entry : entries)
      valuesArray[i++] = entry.second->Value();
    return valuesArray;
  }
}
