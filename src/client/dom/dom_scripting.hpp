#pragma once

#include <string>
#include <memory>
#include <vector>
#include <node/v8.h>

using namespace std;

namespace dom
{
  enum ContextEmbedderIndex
  {
    kSandboxObject = 0x100,
  };

  class DOMScriptingContext
  {
  private:
    static void PropertyGetterCallback(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      auto isolate = info.GetIsolate();
      auto context = isolate->GetCurrentContext();
      auto sandbox = context->GetEmbedderData(ContextEmbedderIndex::kSandboxObject).As<v8::Object>();

      v8::MaybeLocal<v8::Value> maybeValue = sandbox->GetRealNamedProperty(context, property);
      if (maybeValue.IsEmpty())
        maybeValue = context->Global()->GetRealNamedProperty(context, property);

      v8::Local<v8::Value> resultValue;
      if (maybeValue.ToLocal(&resultValue))
      {
        if (resultValue == sandbox)
          resultValue = context->Global();

        info.GetReturnValue().Set(resultValue);
      }
    }

  public:
    DOMScriptingContext()
        : isolate(v8::Isolate::GetCurrent())
    {
      auto mainContext = isolate->GetCurrentContext();
      v8::Context::Scope context_scope(mainContext);
      v8::HandleScope handle_scope(isolate);

      v8::Local<v8::FunctionTemplate> globalFuncTemplate = v8::FunctionTemplate::New(isolate);
      v8::Local<v8::ObjectTemplate> globalObjectTemplate = globalFuncTemplate->InstanceTemplate();

      v8::NamedPropertyHandlerConfiguration namedConfig(
          PropertyGetterCallback,
          nullptr,
          nullptr,
          nullptr,
          nullptr,
          nullptr,
          nullptr,
          {},
          v8::PropertyHandlerFlags::kHasNoSideEffect);
      globalObjectTemplate->SetHandler(namedConfig);

      auto newContext = v8::Context::New(isolate, nullptr, globalObjectTemplate);
      auto global = mainContext->Global();
      auto sandbox = v8::Object::New(isolate);
      {
#define V8_SET_GLOBAL_FROM_MAIN(name)                                                                                 \
  do                                                                                                                  \
  {                                                                                                                   \
    auto value = global->Get(mainContext, v8::String::NewFromUtf8(isolate, #name).ToLocalChecked()).ToLocalChecked(); \
    sandbox->Set(mainContext, v8::String::NewFromUtf8(isolate, #name).ToLocalChecked(), value).FromJust();            \
  } while (0)
        // Set the global object for scripting
        V8_SET_GLOBAL_FROM_MAIN(console);
        V8_SET_GLOBAL_FROM_MAIN(URL);
        V8_SET_GLOBAL_FROM_MAIN(Blob);
#undef V8_SET_GLOBAL_FROM_MAIN
      }
      newContext->SetEmbedderData(ContextEmbedderIndex::kSandboxObject, sandbox);
      newContext->SetSecurityToken(mainContext->GetSecurityToken());
      scriptingContext.Reset(isolate, newContext);
    }

  public:
    void compile(const std::string &source)
    {
      auto context = scriptingContext.Get(isolate);
      v8::Isolate::Scope isolate_scope(isolate);
      v8::Context::Scope context_scope(context);
      v8::HandleScope handle_scope(isolate);

      // create a new script
      auto sourceString = v8::String::NewFromUtf8(isolate, source.c_str()).ToLocalChecked();
      auto maybeScript = v8::Script::Compile(context, sourceString);

      v8::Local<v8::Script> script;
      if (maybeScript.ToLocal(&script))
        scripts.push_back(v8::Global<v8::Script>(isolate, script));
      else
        fprintf(stderr, "Failed to compile script\n"); // TODO: throw exception?
    }
    void run()
    {
      auto context = scriptingContext.Get(isolate);
      v8::Isolate::Scope isolate_scope(isolate);
      v8::Context::Scope context_scope(context);
      v8::HandleScope handle_scope(isolate);

      // run all scripts
      for (auto &script : scripts)
      {
        script.Get(isolate)->Run(context).ToLocalChecked();
      }
    }

  private:
    v8::Isolate *isolate;
    v8::Global<v8::Context> scriptingContext;
    vector<v8::Global<v8::Script>> scripts;
  };
}
