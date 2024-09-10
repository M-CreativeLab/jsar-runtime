#include <assert.h>
#include "./dom_scripting.hpp"
#include "idgen.hpp"

using namespace std;

namespace dom
{
  void DOMScriptingContext::PropertyGetterCallback(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value> &info)
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

  DOMScriptingContext::DOMScriptingContext()
      : isolate(v8::Isolate::GetCurrent())
  {
  }

  void DOMScriptingContext::setDocumentValue(v8::Local<v8::Value> value)
  {
    auto mainContext = isolate->GetCurrentContext();
    v8::Isolate::Scope isolate_scope(isolate);
    v8::Context::Scope context_scope(mainContext);
    {
      documentValue.Reset(isolate, value);
    }
  }

  void DOMScriptingContext::makeV8Context()
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

  shared_ptr<DOMScript> DOMScriptingContext::create(const string &url, SourceTextType type)
  {
    assert(isContextInitialized);
    shared_ptr<DOMScript> script;
    switch (type)
    {
    case SourceTextType::Classic:
      script = make_shared<DOMClassicScript>(shared_from_this());
      break;
    case SourceTextType::ESM:
      script = make_shared<DOMModule>(shared_from_this());
      break;
    default:
      break;
    }
    script->url = url;
    scripts.insert({script->id, script});
    return script;
  }

  bool DOMScriptingContext::compile(shared_ptr<DOMScript> script, const std::string &source)
  {
    assert(isContextInitialized);
    auto context = scriptingContext.Get(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope contextScope(context);
    return script->compile(isolate, source);
  }

  void DOMScriptingContext::evaluate(shared_ptr<DOMScript> script)
  {
    assert(isContextInitialized);

    // Check if the script is in the list
    auto it = scripts.find(script->id);
    if (it == scripts.end())
      return;

    auto context = scriptingContext.Get(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope contextScope(context);
    script->evaluate(isolate);
  }

  DOMScript::DOMScript()
  {
    static TrIdGenerator scriptIdGen(0xf9);
    id = scriptIdGen.get();
  }

  DOMClassicScript::DOMClassicScript(shared_ptr<DOMScriptingContext> context)
      : DOMScript()
  {
  }

  DOMClassicScript::~DOMClassicScript()
  {
    scriptStore.Reset();
  }

  bool DOMClassicScript::compile(v8::Isolate *isolate, const string &sourceStr)
  {
    v8::HandleScope scope(isolate);

    // create the script origin
    auto urlString = v8::String::NewFromUtf8(isolate, url.c_str()).ToLocalChecked();
    int lineOffset = 0;
    int columnOffset = 0;
    v8::ScriptOrigin origin(isolate,
                            urlString,
                            lineOffset,
                            columnOffset,
                            crossOrigin,
                            id,
                            v8::Local<v8::Value>(),
                            false,
                            false,
                            false);

    // create the script
    auto sourceString = v8::String::NewFromUtf8(isolate, sourceStr.c_str()).ToLocalChecked();
    v8::ScriptCompiler::Source source(sourceString, origin);
    v8::ScriptCompiler::CompileOptions options = v8::ScriptCompiler::kNoCompileOptions;

    // compile the script
    auto context = isolate->GetCurrentContext();
    auto maybeScript = v8::ScriptCompiler::Compile(context, &source, options);

    v8::Local<v8::Script> script;
    if (maybeScript.ToLocal(&script))
    {
      scriptStore.Reset(isolate, script);
      return true;
    }
    else
    {
      fprintf(stderr, "Failed to compile script\n"); // TODO: throw exception?
      return false;
    }
  }

  void DOMClassicScript::evaluate(v8::Isolate *isolate)
  {
    v8::HandleScope scope(isolate);

    auto script = scriptStore.Get(isolate);
    auto context = isolate->GetCurrentContext();
    auto result = script->Run(context);
    if (result.IsEmpty())
      fprintf(stderr, "Failed to run script\n"); // TODO: throw exception?
  }

  v8::MaybeLocal<v8::Module> DOMModule::ResolveModuleCallback(v8::Local<v8::Context> context,
                                                              v8::Local<v8::String> specifier,
                                                              v8::Local<v8::FixedArray> importAssertions,
                                                              v8::Local<v8::Module> referrer)
  {
    v8::Isolate* isolate = context->GetIsolate();
    auto specifier_utf8 = v8::String::Utf8Value(context->GetIsolate(), specifier);
    string specifierStr(*specifier_utf8, specifier_utf8.length());
    fprintf(stderr, "ResolveModuleCallback: %s\n", specifierStr.c_str());

    return v8::MaybeLocal<v8::Module>();
  }

  DOMModule::DOMModule(shared_ptr<DOMScriptingContext> context)
      : DOMScript()
  {
  }

  DOMModule::~DOMModule()
  {
    moduleStore.Reset();
  }

  bool DOMModule::compile(v8::Isolate *isolate, const string &sourceStr)
  {
    v8::HandleScope scope(isolate);

    // create the script origin
    auto urlString = v8::String::NewFromUtf8(isolate, url.c_str()).ToLocalChecked();
    int lineOffset = 0;
    int columnOffset = 0;
    v8::ScriptOrigin origin(isolate,
                            urlString,
                            lineOffset,
                            columnOffset,
                            crossOrigin,
                            id,
                            v8::Local<v8::Value>(),
                            false,
                            false,
                            true);

    // create the script
    auto sourceString = v8::String::NewFromUtf8(isolate, sourceStr.c_str()).ToLocalChecked();
    v8::ScriptCompiler::Source source(sourceString, origin);
    v8::ScriptCompiler::CompileOptions options = v8::ScriptCompiler::kNoCompileOptions;

    // compile the script
    v8::Local<v8::Module> module;
    auto context = isolate->GetCurrentContext();

    if (!v8::ScriptCompiler::CompileModule(isolate, &source, options).ToLocal(&module))
    {
      fprintf(stderr, "Failed to compile module\n"); // TODO: throw exception?
      return false;
    }
    else
    {
      // TODO: compile the modules via the module graph
      moduleStore.Reset(isolate, module);
      return true;
    }
  }

  void DOMModule::evaluate(v8::Isolate *isolate)
  {
    v8::HandleScope scope(isolate);
    auto module = moduleStore.Get(isolate);
    auto context = isolate->GetCurrentContext();

    if (!instantiate(isolate))
      return;

    v8::Local<v8::Value> resultValue;
    {
      v8::TryCatch tryCatch(isolate);
      if (!module->Evaluate(context).ToLocal(&resultValue) || resultValue.IsEmpty())
      {
        if (tryCatch.HasCaught())
        {
          if (tryCatch.HasTerminated())
            tryCatch.ReThrow();
          return;
        }
      }
    }
  }

  bool DOMModule::instantiate(v8::Isolate *isolate)
  {
    v8::HandleScope scope(isolate);
    auto module = moduleStore.Get(isolate);
    auto context = isolate->GetCurrentContext();
    v8::TryCatch tryCatch(isolate);

    bool r = false;
    if (!module->InstantiateModule(context, ResolveModuleCallback).To(&r) || !r)
      return false;

    if (tryCatch.HasCaught() && !tryCatch.HasTerminated())
    {
      assert(tryCatch.Message().IsEmpty() != true);
      assert(tryCatch.Exception().IsEmpty() != true);
      tryCatch.ReThrow();
      return false;
    }
    return true;
  }
}
