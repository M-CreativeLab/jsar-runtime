#include <iostream>
#include "./worker_context.hpp"

using namespace std;

namespace dombinding
{
  Napi::FunctionReference *WorkerContext::constructor;

  void WorkerContext::Init(Napi::Env env, Napi::Object exports)
  {
    auto props = GetClassProperties();
    {
      auto newProps = vector<Napi::ClassPropertyDescriptor<WorkerContext>>({
          InstanceMethod("start", &WorkerContext::Start),
      });
      props.insert(props.end(), newProps.begin(), newProps.end());
    }
    Napi::Function func = DefineClass(env, "WorkerContext", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("WorkerContext", func);
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

    auto scriptSource = info[0].As<Napi::String>().Utf8Value();
    auto script = contextImpl->scriptingContext->create(contextImpl, "", dom::SourceTextType::Classic);
    contextImpl->scriptingContext->compile(script, scriptSource);
    contextImpl->scriptingContext->evaluate(script);
    return env.Undefined();
  }
}
