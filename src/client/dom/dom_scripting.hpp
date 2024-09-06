#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <assert.h>
#include <node/v8.h>
#include "idgen.hpp"

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
    }

  public:
    void setDocumentValue(v8::Local<v8::Value> value)
    {
      auto mainContext = isolate->GetCurrentContext();
      v8::Isolate::Scope isolate_scope(isolate);
      v8::Context::Scope context_scope(mainContext);
      {
        documentValue.Reset(isolate, value);
      }
    }
    void makeContext()
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
#define V8_SET_GLOBAL_FROM_VALUE(name, value) \
  sandbox->Set(mainContext, v8::String::NewFromUtf8(isolate, #name).ToLocalChecked(), value).FromJust()
#define V8_SET_GLOBAL_FROM_MAIN(name)                                                                                 \
  do                                                                                                                  \
  {                                                                                                                   \
    auto value = global->Get(mainContext, v8::String::NewFromUtf8(isolate, #name).ToLocalChecked()).ToLocalChecked(); \
    V8_SET_GLOBAL_FROM_VALUE(name, value);                                                                            \
  } while (0)

        // Set the global object for scripting
        V8_SET_GLOBAL_FROM_MAIN(URL);
        V8_SET_GLOBAL_FROM_MAIN(Blob);
        V8_SET_GLOBAL_FROM_MAIN(console);

        // Expose the WebGL and WebGL2 contexts
        V8_SET_GLOBAL_FROM_MAIN(WebGLRenderingContext);
        V8_SET_GLOBAL_FROM_MAIN(WebGL2RenderingContext);

        // Expose the WebXR Device API
        V8_SET_GLOBAL_FROM_MAIN(XRRigidTransform);
        V8_SET_GLOBAL_FROM_MAIN(XRWebGLLayer);

        // Expose the specific objects
        if (!documentValue.IsEmpty())
          V8_SET_GLOBAL_FROM_VALUE(document, documentValue.Get(isolate).As<v8::Object>());
#undef V8_SET_GLOBAL_FROM_MAIN
#undef V8_SET_GLOBAL_FROM_VALUE
      }
      newContext->SetEmbedderData(ContextEmbedderIndex::kSandboxObject, sandbox);
      newContext->SetSecurityToken(mainContext->GetSecurityToken());
      scriptingContext.Reset(isolate, newContext);
      isContextInitialized = true;
    }
    uint32_t compile(const std::string &source)
    {
      assert(isContextInitialized);
      auto context = scriptingContext.Get(isolate);
      v8::Isolate::Scope isolate_scope(isolate);
      v8::Context::Scope context_scope(context);
      v8::HandleScope handle_scope(isolate);

      // create a new script
      auto sourceString = v8::String::NewFromUtf8(isolate, source.c_str()).ToLocalChecked();
      auto maybeScript = v8::Script::Compile(context, sourceString);

      v8::Local<v8::Script> script;
      if (!maybeScript.ToLocal(&script))
      {
        fprintf(stderr, "Failed to compile script\n"); // TODO: throw exception?
        return 0;
      }
      static TrIdGenerator scriptIdGen(0xf9);
      auto scriptId = scriptIdGen.get();
      scripts.insert({scriptId, v8::Global<v8::Script>(isolate, script)});
      return scriptId;
    }
    void run(uint32_t scriptId)
    {
      assert(isContextInitialized);
      auto context = scriptingContext.Get(isolate);
      v8::Isolate::Scope isolate_scope(isolate);
      v8::Context::Scope context_scope(context);
      v8::HandleScope handle_scope(isolate);

      auto script = scripts[scriptId].Get(isolate);
      auto result = script->Run(context);
      if (result.IsEmpty())
      {
        fprintf(stderr, "Failed to run script\n");
        return;
      }
    }

  private:
    v8::Isolate *isolate;
    v8::Global<v8::Value> documentValue;
    v8::Global<v8::Context> scriptingContext;
    map<uint32_t, v8::Global<v8::Script>> scripts;
    bool isContextInitialized = false;
  };
}
