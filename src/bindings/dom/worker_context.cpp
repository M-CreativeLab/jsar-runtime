#include <iostream>
#include "./worker_context.hpp"

using namespace std;

namespace dombinding
{
  thread_local Napi::FunctionReference *WorkerContext::constructor;
  void WorkerContext::Init(Napi::Env env, Napi::Object exports)
  {
    auto props = GetClassProperties();
    {
      auto newProps = vector<Napi::ClassPropertyDescriptor<WorkerContext>>({
          InstanceMethod("start", &WorkerContext::Start),
          InstanceMethod("startFromSource", &WorkerContext::StartFromSource),
          InstanceMethod("dispatchEvent", &WorkerContext::DispatchEvent),
      });
      props.insert(props.end(), newProps.begin(), newProps.end());
    }
    Napi::Function func = DefineClass(env, "WorkerContext", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("WorkerContext", func);
  }

  WorkerContext::WorkerContext(const Napi::CallbackInfo &info) : RuntimeContextBase<WorkerContext, dom::WorkerContext>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsString())
    {
      Napi::TypeError::New(env, "Expected a string.").ThrowAsJavaScriptException();
      return;
    }

    auto baseURI = info[0].As<Napi::String>().Utf8Value();
    contextImpl->setBaseURI(baseURI);

    if (info.Length() >= 2 && info[1].IsObject())
    {
      auto jsOptionsObject = info[1].As<Napi::Object>();
      if (jsOptionsObject.Has("type") && jsOptionsObject.Get("type").IsString())
      {
        auto workerScriptType = jsOptionsObject.Get("type").ToString().Utf8Value();
        if (workerScriptType == "module")
          contextImpl->sourceType = dom::SourceTextType::ESM;
      }
      if (jsOptionsObject.Has("credentials") && jsOptionsObject.Get("credentials").IsString())
      {
        auto credentials = jsOptionsObject.Get("credentials").ToString().Utf8Value();
        if (credentials == "omit")
          contextImpl->credentials = dom::WorkerCredentials::Omit;
        else if (credentials == "same-origin")
          contextImpl->credentials = dom::WorkerCredentials::SameOrigin;
        else if (credentials == "include")
          contextImpl->credentials = dom::WorkerCredentials::Include;
      }
      if (jsOptionsObject.Has("name") && jsOptionsObject.Get("name").IsString())
      {
        contextImpl->workerName = jsOptionsObject.Get("name").ToString().Utf8Value();
      }
    }
  }

  Napi::Value WorkerContext::Start(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 0 && !info[0].IsString())
    {
      Napi::TypeError::New(env, "Expected a string.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    // Make the worker's context
    auto scriptingContext = contextImpl->scriptingContext;
    scriptingContext->makeWorkerContext();

    auto scriptUrl = info[0].As<Napi::String>().Utf8Value();
    contextImpl->start(scriptUrl);
    return env.Undefined();
  }

  Napi::Value WorkerContext::StartFromSource(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 0 && !info[0].IsString())
    {
      Napi::TypeError::New(env, "source is required.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    // Make the worker's context
    auto scriptingContext = contextImpl->scriptingContext;
    scriptingContext->makeWorkerContext();

    auto scriptSource = info[0].As<Napi::String>().Utf8Value();
    contextImpl->startFromSource(scriptSource);
    return env.Undefined();
  }

  Napi::Value WorkerContext::DispatchEvent(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 0 && !info[0].IsObject())
    {
      Napi::TypeError::New(env, "event is required.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto jsEvent = info[0].As<Napi::Object>();
    contextImpl->dispatchEvent(convertNapiValueToV8Local<v8::Object>(jsEvent));
    return env.Undefined();
  }
}
