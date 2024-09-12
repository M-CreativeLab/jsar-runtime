#include <assert.h>
#include "crates/jsar_jsbindings.h"
#include "./dom_scripting.hpp"
#include "./rendering_context.hpp"
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
    else
    {
      info.GetReturnValue().SetUndefined();
    }
  }

  v8::MaybeLocal<v8::Promise> DOMScriptingContext::ImportModuleDynamicallyCallback(v8::Local<v8::Context> context,
                                                                                   v8::Local<v8::Data> hostDefinedOptions,
                                                                                   v8::Local<v8::Value> resourceName,
                                                                                   v8::Local<v8::String> specifier,
                                                                                   v8::Local<v8::FixedArray> importAssertions)
  {
    auto isolate = context->GetIsolate();
    v8::EscapableHandleScope handleScope(isolate);

    v8::Local<v8::FixedArray> options = hostDefinedOptions.As<v8::FixedArray>();
    DOMScriptingContext *scriptingContext = nullptr;
    {
      auto externalObject = context->GetEmbedderData(ContextEmbedderIndex::kEnvironmentObject).As<v8::External>();
      if (!externalObject.IsEmpty())
        scriptingContext = static_cast<DOMScriptingContext *>(externalObject->Value());
    }
    if (scriptingContext == nullptr)
    {
      std::cerr << "Failed to get the scripting context object" << std::endl;
      return v8::MaybeLocal<v8::Promise>();
    }

    v8::String::Utf8Value specifier_utf8(isolate, specifier);
    string specifierStr(*specifier_utf8, specifier_utf8.length());

    auto type = options->Get(context, HostDefinedOptions::kType).As<v8::Number>()->Int32Value(context).ToChecked();
    auto id = options->Get(context, HostDefinedOptions::kID).As<v8::Number>()->Int32Value(context).ToChecked();

    if (type != (int)SourceTextType::ESM)
    {
      v8::Local<v8::Promise::Resolver> resolver;
      if (!v8::Promise::Resolver::New(context).ToLocal(&resolver))
        return v8::MaybeLocal<v8::Promise>();

      auto typeError = v8::Exception::TypeError(v8::String::NewFromUtf8(isolate, "dynamic import() is only supported in ECMAScript module.").ToLocalChecked());
      resolver->Reject(context, typeError).ToChecked();
      return handleScope.Escape(resolver->GetPromise());
    }
    else
    {
      shared_ptr<DOMModule> dependent = nullptr;
      {
        auto &map = scriptingContext->idToModuleMap;
        if (!map.empty())
        {
          auto it = map.find(id);
          if (it != map.end())
            dependent = it->second;
        }
      }
      if (dependent == nullptr)
      {
        std::cerr << "request for '" << specifierStr << "' is from invalid module" << std::endl;
        return v8::MaybeLocal<v8::Promise>();
      }

      auto moduleUrl = crates::jsar::UrlHelper::CreateUrlStringWithPath(dependent->url, specifierStr);
      if (moduleUrl == "")
      {
        std::cerr << "Failed to create URL for '" << specifierStr << "'" << std::endl;
        return v8::MaybeLocal<v8::Promise>();
      }

      v8::Local<v8::Promise::Resolver> resolver;
      if (!v8::Promise::Resolver::New(context).ToLocal(&resolver))
        return v8::MaybeLocal<v8::Promise>();

      auto renderingContext = dependent->documentRenderingContext.lock();
      auto script = renderingContext->createScript(moduleUrl, SourceTextType::ESM);
      auto specifierRef = make_shared<string>(specifierStr);

      shared_ptr<v8::Persistent<v8::Promise::Resolver>> persistentResolver = make_shared<v8::Persistent<v8::Promise::Resolver>>(isolate, resolver);
      auto onSourceLoaded = [persistentResolver, isolate, renderingContext, script](const string &source)
      {
        auto moduleRef = dynamic_pointer_cast<DOMModule>(script);
        moduleRef->linkFinishedCallback = [persistentResolver, isolate, renderingContext, script]()
        {
          v8::Isolate::Scope isolateScope(isolate);
          v8::HandleScope handleScope(isolate);
          auto resolver = persistentResolver->Get(isolate);
          renderingContext->scriptingContext->evaluate(script);
          // TODO: support resolve the module exports
          resolver->Resolve(isolate->GetCurrentContext(), v8::Undefined(isolate)).ToChecked();
          persistentResolver->Reset();
        };
        renderingContext->scriptingContext->compile(script, source);
      };
      renderingContext->fetchTextSourceResource(moduleUrl, onSourceLoaded);
      return handleScope.Escape(resolver->GetPromise());
    }
  }

  DOMScriptingContext::DOMScriptingContext()
      : isolate(v8::Isolate::GetCurrent())
  {
  }

  void DOMScriptingContext::enableDynamicImport()
  {
    auto mainContext = isolate->GetCurrentContext();
    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope contextScope(mainContext);
    {
      isolate->SetHostImportModuleDynamicallyCallback(ImportModuleDynamicallyCallback);
    }
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
#define V8_SET_GLOBAL_FROM_MAIN(name)                                                                     \
  do                                                                                                      \
  {                                                                                                       \
    v8::Local<v8::Value> valueToSet;                                                                      \
    auto maybeValue = global->Get(mainContext, v8::String::NewFromUtf8(isolate, #name).ToLocalChecked()); \
    if (!maybeValue.IsEmpty() && maybeValue.ToLocal(&valueToSet))                                         \
    {                                                                                                     \
      V8_SET_GLOBAL_FROM_VALUE(name, valueToSet);                                                         \
    }                                                                                                     \
  } while (0)

      /**
       * Configure the global objects and functions for the DOM scripting.
       */

      // Baisc objects
      V8_SET_GLOBAL_FROM_MAIN(URL);
      V8_SET_GLOBAL_FROM_MAIN(Blob);
      V8_SET_GLOBAL_FROM_MAIN(console);

      // Typed arrays
      V8_SET_GLOBAL_FROM_MAIN(Int8Array);
      V8_SET_GLOBAL_FROM_MAIN(Uint8Array);
      V8_SET_GLOBAL_FROM_MAIN(Uint8ClampedArray);
      V8_SET_GLOBAL_FROM_MAIN(Int16Array);
      V8_SET_GLOBAL_FROM_MAIN(Uint16Array);
      V8_SET_GLOBAL_FROM_MAIN(Int32Array);
      V8_SET_GLOBAL_FROM_MAIN(Uint32Array);
      V8_SET_GLOBAL_FROM_MAIN(Float32Array);
      V8_SET_GLOBAL_FROM_MAIN(Float64Array);

      // Animation frame provider
      V8_SET_GLOBAL_FROM_MAIN(requestAnimationFrame);
      V8_SET_GLOBAL_FROM_MAIN(cancelAnimationFrame);

      // WebGL objects
      V8_SET_GLOBAL_FROM_MAIN(WebGLRenderingContext);
      V8_SET_GLOBAL_FROM_MAIN(WebGL2RenderingContext);

      // WebXR Device API
      V8_SET_GLOBAL_FROM_MAIN(XRRigidTransform);
      V8_SET_GLOBAL_FROM_MAIN(XRWebGLLayer);

      // Expose the new global objects for the spatial application
      /**
       * `gl`: The WEBGLRenderingContext/WEBGL2RenderingContext object for rendering the spatial objects.
       */
      V8_SET_GLOBAL_FROM_MAIN(gl);

      // Specific objects, such as: `document`, `window`, etc.
      if (!documentValue.IsEmpty())
        V8_SET_GLOBAL_FROM_VALUE(document, documentValue.Get(isolate).As<v8::Object>());

      v8::Local<v8::Object> windowObject = v8::Object::New(isolate);
      {
#define V8_SET_WINDOW_FROM_MAIN(name)                                                                                  \
  do                                                                                                                   \
  {                                                                                                                    \
    v8::Local<v8::Value> valueToSet;                                                                                   \
    auto maybeValue = global->Get(mainContext, v8::String::NewFromUtf8(isolate, #name).ToLocalChecked());              \
    if (!maybeValue.IsEmpty() && maybeValue.ToLocal(&valueToSet))                                                      \
    {                                                                                                                  \
      windowObject->Set(mainContext, v8::String::NewFromUtf8(isolate, #name).ToLocalChecked(), valueToSet).FromJust(); \
    }                                                                                                                  \
  } while (0)

        // Create window object
        V8_SET_WINDOW_FROM_MAIN(requestAnimationFrame);
        V8_SET_WINDOW_FROM_MAIN(cancelAnimationFrame);
#undef V8_SET_WINDOW_FROM_MAIN
      }
      V8_SET_GLOBAL_FROM_VALUE(window, windowObject);
      V8_SET_GLOBAL_FROM_VALUE(self, windowObject);

#undef V8_SET_GLOBAL_FROM_MAIN
#undef V8_SET_GLOBAL_FROM_VALUE
    }
    newContext->SetEmbedderData(ContextEmbedderIndex::kMagicIndex, v8::Number::New(isolate, 0x5432));
    newContext->SetEmbedderData(ContextEmbedderIndex::kSandboxObject, sandbox);
    newContext->SetEmbedderData(ContextEmbedderIndex::kEnvironmentObject, v8::External::New(isolate, this));
    newContext->SetSecurityToken(mainContext->GetSecurityToken());
    scriptingContext.Reset(isolate, newContext);
    isContextInitialized = true;
  }

  shared_ptr<DOMScript> DOMScriptingContext::create(shared_ptr<DocumentRenderingContext> context, const string &url, SourceTextType type)
  {
    assert(isContextInitialized);
    shared_ptr<DOMScript> script;
    if (type == SourceTextType::Classic)
    {
      auto classicScript = make_shared<DOMClassicScript>(context);
      classicScript->url = url;
      idToScriptMap.insert({classicScript->id, classicScript});
      script = dynamic_pointer_cast<DOMScript>(classicScript);
    }
    else if (type == SourceTextType::ESM)
    {
      auto module = make_shared<DOMModule>(context);
      module->url = url;
      idToModuleMap.insert({module->id, module});
      script = dynamic_pointer_cast<DOMScript>(module);
    }
    return script;
  }

  bool DOMScriptingContext::compile(shared_ptr<DOMScript> script, const std::string &source)
  {
    assert(isContextInitialized);
    auto context = scriptingContext.Get(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope contextScope(context);

    if (!script->compile(isolate, source))
      return false;

    // If the script is ESM, add it to the module map
    if (script->sourceTextType == SourceTextType::ESM)
    {
      auto module = dynamic_pointer_cast<DOMModule>(script);
      hashToModuleMap.insert({module->getModuleHash(), module});
      module->link(isolate);
    }
    return true;
  }

  void DOMScriptingContext::evaluate(shared_ptr<DOMScript> script)
  {
    assert(isContextInitialized);
    auto context = scriptingContext.Get(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope contextScope(context);
    {
      script->evaluate(isolate);
    }
  }

  shared_ptr<DOMModule> DOMScriptingContext::getModuleFromV8(v8::Local<v8::Module> module)
  {
    auto range = hashToModuleMap.equal_range(module->GetIdentityHash());
    for (auto it = range.first; it != range.second; ++it)
    {
      if (it->second->moduleStore.Get(isolate) == module)
      {
        return it->second;
      }
    }
    return nullptr;
  }

  DOMScript::DOMScript(SourceTextType sourceTextType, shared_ptr<DocumentRenderingContext> context)
      : sourceTextType(sourceTextType),
        documentRenderingContext(context)
  {
    static TrIdGenerator scriptIdGen(0xf9);
    id = scriptIdGen.get();
  }

  DOMClassicScript::DOMClassicScript(shared_ptr<DocumentRenderingContext> context)
      : DOMScript(SourceTextType::Classic, context)
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

    v8::Local<v8::PrimitiveArray> hostDefinedOptions = v8::PrimitiveArray::New(isolate, HostDefinedOptions::kLength);
    hostDefinedOptions->Set(isolate, HostDefinedOptions::kType, v8::Number::New(isolate, (int)SourceTextType::Classic));
    hostDefinedOptions->Set(isolate, HostDefinedOptions::kID, v8::Number::New(isolate, id));

    v8::ScriptOrigin origin(isolate,
                            urlString,
                            lineOffset,
                            columnOffset,
                            crossOrigin,
                            id,
                            v8::Local<v8::Value>(),
                            false,
                            false,
                            false,
                            hostDefinedOptions);

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
    v8::Isolate *isolate = context->GetIsolate();
    DOMScriptingContext *scriptingContext = nullptr;
    {
      auto externalObject = context->GetEmbedderData(ContextEmbedderIndex::kEnvironmentObject).As<v8::External>();
      if (!externalObject.IsEmpty())
        scriptingContext = static_cast<DOMScriptingContext *>(externalObject->Value());
    }
    if (scriptingContext == nullptr)
    {
      fprintf(stderr, "Failed to get the scripting context object\n");
      return v8::MaybeLocal<v8::Module>();
    }

    auto specifier_utf8 = v8::String::Utf8Value(context->GetIsolate(), specifier);
    string specifierStr(*specifier_utf8, specifier_utf8.length());

    auto dependent = scriptingContext->getModuleFromV8(referrer);
    if (dependent == nullptr)
    {
      fprintf(stderr, "request for '%s' is from invalid module\n", specifierStr.c_str());
      return v8::MaybeLocal<v8::Module>();
    }
    if (dependent->resolveCache.count(specifierStr) != 1)
    {
      fprintf(stderr, "request for '%s' is not in cache\n", specifierStr.c_str());
      return v8::MaybeLocal<v8::Module>();
    }

    auto module = dependent->resolveCache[specifierStr];
    return module->moduleStore.Get(isolate);
  }

  DOMModule::DOMModule(shared_ptr<DocumentRenderingContext> context)
      : DOMScript(SourceTextType::ESM, context)
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

    v8::Local<v8::PrimitiveArray> hostDefinedOptions = v8::PrimitiveArray::New(isolate, HostDefinedOptions::kLength);
    hostDefinedOptions->Set(isolate, HostDefinedOptions::kType, v8::Number::New(isolate, (int)SourceTextType::ESM));
    hostDefinedOptions->Set(isolate, HostDefinedOptions::kID, v8::Number::New(isolate, id));

    v8::ScriptOrigin origin(isolate,
                            urlString,
                            lineOffset,
                            columnOffset,
                            crossOrigin,
                            id,
                            v8::Local<v8::Value>(),
                            false,
                            false,
                            true,
                            hostDefinedOptions);

    // create the script
    auto sourceString = v8::String::NewFromUtf8(isolate, sourceStr.c_str()).ToLocalChecked();
    v8::ScriptCompiler::Source source(sourceString, origin);
    v8::ScriptCompiler::CompileOptions options = v8::ScriptCompiler::kNoCompileOptions;

    // compile the script
    v8::Local<v8::Module> module;
    auto context = isolate->GetCurrentContext();

    if (!v8::ScriptCompiler::CompileModule(isolate, &source, options).ToLocal(&module))
    {
      fprintf(stderr, "#\n");
      fprintf(stderr, "# Occurred compilitation error\n");
      fprintf(stderr, "# URL: %s\n", url.c_str());
      fprintf(stderr, "#\n");
      fprintf(stderr, "%s\n", sourceStr.c_str());
      return false;
    }
    else
    {
      moduleStore.Reset(isolate, module);
      return true;
    }
  }

  void DOMModule::evaluate(v8::Isolate *isolate)
  {
    if (linked)
    {
      doEvaluate(isolate);
    }
    else
    {
      evaluationScheduled = true;
    }
  }

  int DOMModule::getModuleHash()
  {
    assert(!moduleStore.IsEmpty());
    auto module = moduleStore.Get(v8::Isolate::GetCurrent());
    return module->GetIdentityHash();
  }

  void DOMModule::link(v8::Isolate *isolate)
  {
    v8::HandleScope scope(isolate);
    auto module = moduleStore.Get(isolate);
    auto v8Context = isolate->GetCurrentContext();
    auto renderingContext = documentRenderingContext.lock();

    // Get module requests
    vector<ModuleRequestInfo> moduleRequestInfos;
    auto moduleRequests = module->GetModuleRequests();
    for (int i = 0; i < moduleRequests->Length(); i++)
    {
      auto moduleRequest = moduleRequests->Get(v8Context, i).As<v8::ModuleRequest>();
      auto specifier = moduleRequest->GetSpecifier();
      string specifier_utf8(*v8::String::Utf8Value(isolate, specifier));

      auto moduleUrl = crates::jsar::UrlHelper::CreateUrlStringWithPath(url, specifier_utf8);
      if (moduleUrl != "")
        moduleRequestInfos.push_back({specifier_utf8, moduleUrl});
    }

    validModuleRequestsCount = moduleRequestInfos.size();
    if (validModuleRequestsCount == 0)
    {
      onLinkFinished();
    }
    else
    {
      for (auto moduleRequestInfo : moduleRequestInfos)
      {
        auto script = renderingContext->createScript(moduleRequestInfo.url, SourceTextType::ESM);
        renderingContext->fetchTextSourceResource(moduleRequestInfo.url, [this, script, moduleRequestInfo](const string &source)
                                                  { handleModuleRequestSource(script, moduleRequestInfo.specifier, source); });
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

  void DOMModule::handleModuleRequestSource(shared_ptr<DOMScript> script, const string &specifier, const string &source)
  {
    auto renderingContext = documentRenderingContext.lock();
    /**
     * FIXME: Create references to the `specifier` and `script` objects to prevent them from being destroyed when the callback is called.
     */
    auto specifierRef = make_shared<string>(specifier);
    auto moduleRef = dynamic_pointer_cast<DOMModule>(script);
    moduleRef->linkFinishedCallback = [this, specifierRef, moduleRef]()
    {
      resolveCache.insert({specifierRef->c_str(), moduleRef});
      validModuleRequestsCount--;
      checkLinkFinished();
    };
    renderingContext->scriptingContext->compile(script, source);
  }

  void DOMModule::checkLinkFinished()
  {
    if (validModuleRequestsCount == 0)
      onLinkFinished();
  }

  void DOMModule::onLinkFinished()
  {
    linked = true;
    if (linkFinishedCallback)
      linkFinishedCallback();
    if (evaluationScheduled)
      doEvaluate(v8::Isolate::GetCurrent());
  }

  void DOMModule::doEvaluate(v8::Isolate *isolate)
  {
    if (evaluatedOnce)
      return;
    evaluatedOnce = true;

    v8::HandleScope scope(isolate);
    auto module = moduleStore.Get(isolate);
    auto context = isolate->GetCurrentContext();

    if (!instantiate(isolate))
      return;

    v8::Local<v8::Value> resultValue;
    {
      v8::TryCatch tryCatch(isolate);
      if (!module->Evaluate(context).ToLocal(&resultValue))
      {
        if (tryCatch.HasCaught())
        {
          if (tryCatch.HasTerminated())
            tryCatch.ReThrow();
          return;
        }
      }
      else
      {
        if (resultValue->IsPromise())
        {
          auto promise = v8::Local<v8::Promise>::Cast(resultValue);
          auto resolve = v8::Function::New(context, [](const v8::FunctionCallbackInfo<v8::Value> &info)
                                           { info.GetReturnValue().Set(info[0]); })
                             .ToLocalChecked();
          auto reject = v8::Function::New(context, [](const v8::FunctionCallbackInfo<v8::Value> &info)
                                          {
                                            v8::String::Utf8Value message(info.GetIsolate(), info[0]);
                                            fprintf(stderr, "Failed to execute script: %s\n", *message); })
                            .ToLocalChecked();
          promise->Then(context, resolve, reject).ToLocalChecked();
        }
      }
    }
  }
}
