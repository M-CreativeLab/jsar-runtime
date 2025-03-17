#include <client/scripting_base/v8_utils.hpp>

#include "./mutation_observer.hpp"
#include "./mutation_record.hpp"
#include "./node.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *MutationObserver::constructor = nullptr;
  void MutationObserver::Init(Napi::Env env)
  {
#define MODULE_NAME "MutationObserver"
    Napi::HandleScope scope(env);
    Napi::Function func = DefineClass(env, MODULE_NAME,
                                      {
                                          InstanceMethod("disconnect", &MutationObserver::Disconnect),
                                          InstanceMethod("observe", &MutationObserver::Observe),
                                          InstanceMethod("takeRecords", &MutationObserver::TakeRecords),
                                      });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    (*constructor).SuppressDestruct();

    env.Global().Set(MODULE_NAME, func);
#undef MODULE_NAME
  }

  void MutationObserver::MicrotaskHandler(v8::Isolate *isolate, void *data)
  {
    if (TR_LIKELY(data != nullptr))
    {
      MutationObserver *observer = reinterpret_cast<MutationObserver *>(data);
      observer->dispatchRecordsCallback(observer->callbackEnv_);
    }
  }

  MutationObserver::MutationObserver(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<MutationObserver>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 0 || !info[0].IsFunction())
    {
      string msg = "Failed to construct 'MutationObserver': 1 argument required, but only 0 present.";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return;
    }

    Napi::Function callback = info[0].As<Napi::Function>();
    callbackRef_.Reset(callback);
    observer_ = std::make_shared<dom::MutationObserver>(nullptr);
  }

  MutationObserver::~MutationObserver()
  {
    stopRecordsDispatcher();
  }

  Napi::Value MutationObserver::Disconnect(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    stopRecordsDispatcher(); // Stop the records no matter what it is.
    if (observer_ == nullptr)
    {
      Napi::Error::New(env, "The mutation observer is already disconnected.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    observer_->disconnect();
    observer_.reset();
    return env.Undefined();
  }

  Napi::Value MutationObserver::Observe(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (observer_ == nullptr)
    {
      Napi::Error::New(env, "The mutation observer is already disconnected.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (info.Length() == 0 || !info[0].IsObject())
    {
      Napi::TypeError::New(env, "A target node is required.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (info.Length() < 2 || !info[1].IsObject())
    {
      Napi::TypeError::New(env, "The options object is required.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Object target = info[0].As<Napi::Object>();
    dom::MutationObserver::ObserveOptions options;

    shared_ptr<dom::Node> targetNode = Node::GetImpl(target);
    if (targetNode == nullptr)
    {
      Napi::TypeError::New(env, "The target node must be a valid Node object.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    Napi::Object optionsObject = info[1].As<Napi::Object>();
    {
      if (optionsObject.Has("subtree"))
        options.subtree = optionsObject.Get("subtree").ToBoolean();
      if (optionsObject.Has("childList"))
        options.childList = optionsObject.Get("childList").ToBoolean();
      if (optionsObject.Has("attributes"))
        options.attributes = optionsObject.Get("attributes").ToBoolean();
      if (optionsObject.Has("attributeFilter") && optionsObject.Get("attributeFilter").IsArray())
      {
        Napi::Array attributeFilterArray = optionsObject.Get("attributeFilter").As<Napi::Array>();
        for (size_t i = 0; i < attributeFilterArray.Length(); i++)
          options.attributeFilter.push_back(attributeFilterArray.Get(i).ToString());
      }
      if (optionsObject.Has("attributeOldValue"))
        options.attributeOldValue = optionsObject.Get("attributeOldValue").ToBoolean();
      if (optionsObject.Has("characterData"))
        options.characterData = optionsObject.Get("characterData").ToBoolean();
      if (optionsObject.Has("characterDataOldValue"))
        options.characterDataOldValue = optionsObject.Get("characterDataOldValue").ToBoolean();
    }

    if (!observer_->observe(targetNode, options))
    {
      Napi::Error::New(env, "Failed to observe the target node.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    startRecordsDispatcher(env); // Starts the records dispatcher if the observing operation is finished.
    return env.Undefined();
  }

  Napi::Value MutationObserver::TakeRecords(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (observer_ == nullptr)
    {
      Napi::Error::New(env, "The mutation observer is already disconnected.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    vector<dom::MutationRecord> records = observer_->takeRecords();
    Napi::Array recordsArray = Napi::Array::New(env, records.size());
    for (size_t i = 0; i < records.size(); i++)
      recordsArray.Set(i, MutationRecord::MakeFromImpl(env, records[i]));
    return recordsArray;
  }

  void MutationObserver::dispatchRecordsCallback(Napi::Env env)
  {
    vector<dom::MutationRecord> records = observer_->takeRecords();
    if (records.size() > 0)
    {
      Napi::HandleScope scope(env);
      Napi::Function callback = callbackRef_.Value();
      Napi::Array recordsArray = Napi::Array::New(env, records.size());
      for (size_t i = 0; i < records.size(); i++)
        recordsArray.Set(i, MutationRecord::MakeFromImpl(env, records[i]));
      callback.Call(Value(), {recordsArray});
    }
  }

  void MutationObserver::startRecordsDispatcher(Napi::Env env)
  {
    callbackEnv_ = env;

    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    isolate->AddMicrotasksCompletedCallback(MicrotaskHandler, this);
  }

  void MutationObserver::stopRecordsDispatcher()
  {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    isolate->RemoveMicrotasksCompletedCallback(MicrotaskHandler, this);
  }
}
