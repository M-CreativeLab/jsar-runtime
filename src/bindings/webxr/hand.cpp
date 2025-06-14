#include "hand.hpp"
#include "space.hpp"

namespace bindings
{
  using namespace std;
  using namespace Napi;

  thread_local FunctionReference *XRHand::constructor;

  // static
  void XRHand::Init(Napi::Env env)
  {
#define MODULE_NAME "XRHand"
    Napi::Function tpl = DefineClass(
      env,
      MODULE_NAME,
      {
        InstanceMethod("entries", &XRHand::Entries),
        InstanceMethod("forEach", &XRHand::ForEach),
        InstanceMethod("get", &XRHand::Get),
        InstanceMethod("keys", &XRHand::Keys),
        InstanceMethod("values", &XRHand::Values),
      });

    constructor = new FunctionReference();
    *constructor = Persistent(tpl);
    env.SetInstanceData(constructor);
    env.Global().Set(MODULE_NAME, tpl);
#undef MODULE_NAME
  }

  Object XRHand::NewInstance(Napi::Env env, std::shared_ptr<client_xr::XRInputSource> inputSource)
  {
    EscapableHandleScope scope(env);
    client_xr::XRHand hand(inputSource);
    auto handleExternal = External<client_xr::XRHand>::New(env, &hand);
    Object instance = constructor->New({handleExternal});
    return scope.Escape(instance).ToObject();
  }

  XRHand::XRHand(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<XRHand>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsExternal())
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }

    auto external = info[0].As<Napi::External<client_xr::XRHand>>();
    handle_ = *external.Data();

    for (auto jointSpace : handle_->values())
    {
      auto jointSpaceObject = XRJointSpace::NewInstance(env, jointSpace);
      auto nameStr = jointSpaceObject.Get("jointName").ToString().Utf8Value();
      auto jointSpaceObjectRef = new Napi::ObjectReference(Napi::Persistent(jointSpaceObject));
      entries_.insert(std::pair<std::string, Napi::ObjectReference *>(nameStr, jointSpaceObjectRef));
    }
  }

  XRHand::~XRHand()
  {
    for (auto &entry : entries_)
      delete entry.second;
    entries_.clear();
  }

  Napi::Value XRHand::Entries(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto entriesObject = Napi::Object::New(env);
    for (auto &entry : entries_)
      entriesObject.Set(entry.first, entry.second->Value());
    return entriesObject;
  }

  Napi::Value XRHand::ForEach(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto callback = info[0].As<Napi::Function>();
    for (auto &entry : entries_)
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
    auto entry = entries_.find(key);
    if (entry == entries_.end())
      return env.Null();
    return entry->second->Value();
  }

  Napi::Value XRHand::Keys(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto keysArray = Napi::Array::New(env, entries_.size());
    int i = 0;
    for (auto &entry : entries_)
      keysArray[i++] = Napi::String::New(env, entry.first);
    return keysArray;
  }

  Napi::Value XRHand::Values(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto valuesArray = Napi::Array::New(env, entries_.size());
    int i = 0;
    for (auto &entry : entries_)
      valuesArray[i++] = entry.second->Value();
    return valuesArray;
  }
}
