#include <assert.h>
#include <rapidjson/document.h>

#include "crates/jsar_jsbindings.h"
#include "idgen.hpp"
#include "./dom_scripting.hpp"
#include "./runtime_context.hpp"

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

  void DOMScriptingContext::WindowProxyPropertyGetterCallback(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto isolate = info.GetIsolate();
    auto context = isolate->GetCurrentContext();
    auto globalObject = context->Global();

    v8::MaybeLocal<v8::Value> maybeValue = globalObject->Get(context, property);
    /**
     * TODO: add new an embedder data for the window-only properties.
     */

    v8::Local<v8::Value> resultValue;
    if (maybeValue.ToLocal(&resultValue))
      info.GetReturnValue().Set(resultValue);
    else
      info.GetReturnValue().SetUndefined();
  }

  void DOMScriptingContext::WorkerSelfProxyPropertyGetterCallback(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    auto isolate = info.GetIsolate();
    auto context = isolate->GetCurrentContext();
    auto globalObject = context->Global();

    v8::MaybeLocal<v8::Value> maybeValue = globalObject->Get(context, property);
    /**
     * TODO: add new an embedder data for the window-only properties.
     */

    v8::Local<v8::Value> resultValue;
    if (maybeValue.ToLocal(&resultValue))
      info.GetReturnValue().Set(resultValue);
    else
      info.GetReturnValue().SetUndefined();
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

      auto moduleUrl = dependent->getUrlBySpecifier(specifierStr);
      if (moduleUrl == "")
      {
        std::cerr << "Failed to create URL for '" << specifierStr << "'" << std::endl;
        return v8::MaybeLocal<v8::Promise>();
      }

      v8::Local<v8::Promise::Resolver> resolver;
      if (!v8::Promise::Resolver::New(context).ToLocal(&resolver))
        return v8::MaybeLocal<v8::Promise>();

      auto scriptingContext = dependent->runtimeContext.lock()->scriptingContext;
      auto persistentResolver = make_shared<v8::Persistent<v8::Promise::Resolver>>(isolate, resolver);
      auto specifierRef = make_shared<string>(specifierStr);
      auto onModuleLoaded = [isolate, persistentResolver, dependent, scriptingContext, specifierRef](shared_ptr<DOMModule> module)
      {
        v8::Isolate::Scope isolateScope(isolate);
        v8::HandleScope handleScope(isolate);

        auto resolver = persistentResolver->Get(isolate);
        dependent->resolveCache.insert({*specifierRef, module});
        scriptingContext->evaluate(dynamic_pointer_cast<DOMScript>(module));
        // TODO: support resolve the module exports
        resolver->Resolve(isolate->GetCurrentContext(), v8::Undefined(isolate)).ToChecked();
        persistentResolver->Reset();
      };
      scriptingContext->tryImportModule(moduleUrl, false, onModuleLoaded);
      return handleScope.Escape(resolver->GetPromise());
    }
  }

  DOMScriptingContext::DOMScriptingContext(shared_ptr<RuntimeContext> runtimeContext)
      : isolate(v8::Isolate::GetCurrent()), runtimeContext(runtimeContext)
  {
  }

  void DOMScriptingContext::enableDynamicImport()
  {
    auto mainContext = isolate->GetCurrentContext();
    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope contextScope(mainContext);
    isolate->SetHostImportModuleDynamicallyCallback(ImportModuleDynamicallyCallback);
  }

  void DOMScriptingContext::makeMainContext(v8::Local<v8::Value> documentValue)
  {
    assert(!isContextInitialized);
    assert(v8ContextStore.IsEmpty());
    auto mainContext = isolate->GetCurrentContext();
    {
      v8::Isolate::Scope isolateScope(isolate);
      v8::Context::Scope contextScope(mainContext);
      v8::HandleScope handleScope(isolate);

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
        V8_SET_GLOBAL_FROM_MAIN(navigator);
        V8_SET_GLOBAL_FROM_MAIN(console);
        V8_SET_GLOBAL_FROM_MAIN(URL);
        V8_SET_GLOBAL_FROM_MAIN(Blob);
        V8_SET_GLOBAL_FROM_MAIN(TextDecoder);
        V8_SET_GLOBAL_FROM_MAIN(Worker);

        // Global functions
        V8_SET_GLOBAL_FROM_MAIN(atob);
        V8_SET_GLOBAL_FROM_MAIN(btoa);
        V8_SET_GLOBAL_FROM_MAIN(fetch);
        V8_SET_GLOBAL_FROM_MAIN(setTimeout);
        V8_SET_GLOBAL_FROM_MAIN(clearTimeout);
        V8_SET_GLOBAL_FROM_MAIN(setInterval);
        V8_SET_GLOBAL_FROM_MAIN(clearInterval);
        V8_SET_GLOBAL_FROM_MAIN(requestAnimationFrame);
        V8_SET_GLOBAL_FROM_MAIN(cancelAnimationFrame);

        // Fetch API related objects
        V8_SET_GLOBAL_FROM_MAIN(Headers);
        V8_SET_GLOBAL_FROM_MAIN(Request);
        V8_SET_GLOBAL_FROM_MAIN(Response);

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

        /**
         * Re-exposing the following types is to avoid the "instanceof" issues between v8 contexts.
         */
        // Typed arrays
        V8_SET_GLOBAL_FROM_MAIN(Array);
        V8_SET_GLOBAL_FROM_MAIN(ArrayBuffer);
        V8_SET_GLOBAL_FROM_MAIN(Int8Array);
        V8_SET_GLOBAL_FROM_MAIN(Uint8Array);
        V8_SET_GLOBAL_FROM_MAIN(Uint8ClampedArray);
        V8_SET_GLOBAL_FROM_MAIN(Int16Array);
        V8_SET_GLOBAL_FROM_MAIN(Uint16Array);
        V8_SET_GLOBAL_FROM_MAIN(Int32Array);
        V8_SET_GLOBAL_FROM_MAIN(Uint32Array);
        V8_SET_GLOBAL_FROM_MAIN(Float32Array);
        V8_SET_GLOBAL_FROM_MAIN(Float64Array);
        V8_SET_GLOBAL_FROM_MAIN(DataView);

        // Specific objects, such as: `document`, `window`, etc.
        if (!documentValue.IsEmpty())
          V8_SET_GLOBAL_FROM_VALUE(document, documentValue);

#undef V8_SET_GLOBAL_FROM_MAIN
#undef V8_SET_GLOBAL_FROM_VALUE
      }
      newContext->SetEmbedderData(ContextEmbedderIndex::kMagicIndex, v8::Number::New(isolate, 0x5432));
      newContext->SetEmbedderData(ContextEmbedderIndex::kSandboxObject, sandbox);
      newContext->SetEmbedderData(ContextEmbedderIndex::kEnvironmentObject, v8::External::New(isolate, this));
      newContext->SetSecurityToken(mainContext->GetSecurityToken());
      v8ContextStore.Reset(isolate, newContext);
    }

    {
      // Initialize the new context globals.
      auto newContext = v8ContextStore.Get(isolate);
      v8::Context::Scope contextScope(newContext);
      v8::HandleScope handleScope(isolate);

      v8::Local<v8::Value> windowProxy = createWindowProxy(newContext);
      auto global = newContext->Global();
      global->Set(newContext, v8::String::NewFromUtf8(isolate, "window").ToLocalChecked(), windowProxy).FromJust();
      global->Set(newContext, v8::String::NewFromUtf8(isolate, "self").ToLocalChecked(), windowProxy).FromJust();
    }
    isContextInitialized = true;
  }

  void DOMScriptingContext::makeWorkerContext()
  {
    assert(!isContextInitialized);
    assert(v8ContextStore.IsEmpty());
    auto mainContext = isolate->GetCurrentContext();
    {
      v8::Isolate::Scope isolateScope(isolate);
      v8::Context::Scope contextScope(mainContext);
      v8::HandleScope handleScope(isolate);

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

      auto workerContext = v8::Context::New(isolate, nullptr, globalObjectTemplate);
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
         * Configure the WorkerGlobalScope objects and functions.
         * 
         * See https://developer.mozilla.org/en-US/docs/Web/API/WorkerGlobalScope
         */

        // Baisc objects
        V8_SET_GLOBAL_FROM_MAIN(console);
        V8_SET_GLOBAL_FROM_MAIN(URL);
        V8_SET_GLOBAL_FROM_MAIN(Blob);
        V8_SET_GLOBAL_FROM_MAIN(TextDecoder);

        // Global functions
        V8_SET_GLOBAL_FROM_MAIN(atob);
        V8_SET_GLOBAL_FROM_MAIN(btoa);
        V8_SET_GLOBAL_FROM_MAIN(fetch);
        V8_SET_GLOBAL_FROM_MAIN(setTimeout);
        V8_SET_GLOBAL_FROM_MAIN(clearTimeout);
        V8_SET_GLOBAL_FROM_MAIN(setInterval);
        V8_SET_GLOBAL_FROM_MAIN(clearInterval);
        V8_SET_GLOBAL_FROM_MAIN(postMessage);

        // Fetch API related objects
        V8_SET_GLOBAL_FROM_MAIN(Headers);
        V8_SET_GLOBAL_FROM_MAIN(Request);
        V8_SET_GLOBAL_FROM_MAIN(Response);

#undef V8_SET_GLOBAL_FROM_MAIN
#undef V8_SET_GLOBAL_FROM_VALUE
      }
      workerContext->SetEmbedderData(ContextEmbedderIndex::kMagicIndex, v8::Number::New(isolate, 0x6432));
      workerContext->SetEmbedderData(ContextEmbedderIndex::kSandboxObject, sandbox);
      workerContext->SetEmbedderData(ContextEmbedderIndex::kEnvironmentObject, v8::External::New(isolate, this));
      workerContext->SetSecurityToken(mainContext->GetSecurityToken());
      v8ContextStore.Reset(isolate, workerContext);
    }

    {
      auto newContext = v8ContextStore.Get(isolate);
      v8::Context::Scope contextScope(newContext);
      v8::HandleScope handleScope(isolate);
      v8::Local<v8::Value> selfProxy = createWorkerSelfProxy(newContext);
      auto global = newContext->Global();
      global->Set(newContext, v8::String::NewFromUtf8(isolate, "self").ToLocalChecked(), selfProxy).FromJust();
    }
    isContextInitialized = true;
  }

  shared_ptr<DOMScript> DOMScriptingContext::create(shared_ptr<RuntimeContext> runtimeContext, const string &url, SourceTextType type)
  {
    assert(isContextInitialized);
    shared_ptr<DOMScript> script;
    if (type == SourceTextType::Classic)
    {
      auto classicScript = make_shared<DOMClassicScript>(runtimeContext);
      classicScript->url = url;
      idToScriptMap.insert({classicScript->id, classicScript});
      script = dynamic_pointer_cast<DOMScript>(classicScript);
    }
    else if (type == SourceTextType::ESM)
    {
      auto module = make_shared<DOMModule>(runtimeContext);
      module->url = url;
      idToModuleMap.insert({module->id, module});
      urlToModuleMap.insert({url, module});
      script = dynamic_pointer_cast<DOMScript>(module);
    }
    return script;
  }

  bool DOMScriptingContext::compile(shared_ptr<DOMScript> script, const std::string &source)
  {
    assert(isContextInitialized);
    auto context = v8ContextStore.Get(isolate);
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
    auto context = v8ContextStore.Get(isolate);
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

  shared_ptr<DOMModule> DOMScriptingContext::getModuleFromUrl(const string &url)
  {
    auto it = urlToModuleMap.find(url);
    if (it != urlToModuleMap.end())
      return it->second;
    else
      return nullptr;
  }

  bool DOMScriptingContext::updateImportMapFromJSON(const string &json)
  {
    rapidjson::Document importMapDocument;
    importMapDocument.Parse(json.c_str());
    if (importMapDocument.HasParseError())
      return false;

    if (importMapDocument.IsObject())
    {
      if (importMapDocument.HasMember("imports"))
      {
        auto imports = importMapDocument["imports"].GetObject();
        for (auto it = imports.MemberBegin(); it != imports.MemberEnd(); ++it)
        {
          string key = it->name.GetString();
          string value = it->value.GetString();
          // check the key ends with "/"

          if (key.back() == '/')
          {
            /**
             * Only the key and value are both end with "/", it is a prefix match.
             *
             * See: https://developer.mozilla.org/en-US/docs/Web/HTML/Element/script/type/importmap#mapping_path_prefixes
             */
            if (value.back() == '/')
            {
              importPrefixMap.insert({key, value});
            }
          }
          else
          {
            importExactMap.insert({key, value});
          }
        }
      }
      // TODO: support "integrity"
    }
    return true;
  }

  optional<string> DOMScriptingContext::exactMatchImportMap(const string &specifier)
  {
    if (importExactMap.count(specifier) == 1)
      return importExactMap[specifier];
    else
      return nullopt;
  }

  optional<string> DOMScriptingContext::prefixMatchImportMap(const string &specifier)
  {
    for (auto it = importPrefixMap.begin(); it != importPrefixMap.end(); ++it)
    {
      if (specifier.find(it->first) == 0)
      {
        auto prefix = it->second;
        return prefix + specifier.substr(it->first.length());
      }
    }
    return nullopt;
  }

  void DOMScriptingContext::tryImportModule(const string &url, const bool disableCache, function<void(shared_ptr<DOMModule>)> loadedCallback)
  {
    if (!disableCache)
    {
      auto module = getModuleFromUrl(url);
      if (module)
      {
        loadedCallback(module);
        return;
      }
    }
    shared_ptr<DOMScript> script = create(runtimeContext, url, SourceTextType::ESM);
    auto onModuleSourceLoaded = [this, script, loadedCallback](const string &source)
    {
      auto module = dynamic_pointer_cast<DOMModule>(script);
      module->setLinkFinishedCallback(loadedCallback);
      compile(script, source);
    };
    runtimeContext->fetchTextSourceResource(url, onModuleSourceLoaded);
  }

  bool DOMScriptingContext::dispatchEvent(v8::Local<v8::Object> event)
  {
    assert(isContextInitialized);
    assert(!v8ContextStore.IsEmpty());

    auto context = v8ContextStore.Get(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope contextScope(context);

    auto eventType = event->Get(context, v8::String::NewFromUtf8(isolate, "type").ToLocalChecked()).ToLocalChecked();
    auto eventType_utf8 = v8::String::Utf8Value(isolate, eventType);

    auto global = context->Global();
    auto listenerName = "on" + string(*eventType_utf8, eventType_utf8.length());

    auto eventListener = global->Get(context, v8::String::NewFromUtf8(isolate, listenerName.c_str()).ToLocalChecked());
    if (!eventListener.IsEmpty() && eventListener.ToLocalChecked()->IsFunction())
    {
      v8::TryCatch tryCatch(isolate);
      auto function = eventListener.ToLocalChecked().As<v8::Function>();
      v8::Local<v8::Value> argv[] = {event};
      v8::Local<v8::Value> result;
      if (!function->Call(context, global, 1, argv).ToLocal(&result))
      {
        if (tryCatch.HasCaught())
        {
          v8::Local<v8::Message> message = tryCatch.Message();
          v8::String::Utf8Value messageUtf8(isolate, message->Get());
          std::cerr << "Failed to call the event listener: " << listenerName << ", ";
          std::cerr << "occurred an error: " << *messageUtf8 << std::endl;
        }
        else
        {
          std::cerr << "Failed to call the event listener: " << listenerName << std::endl;
        }
      }
    }
    // TODO: call dispatchEvent?
    return true;
  }

  v8::Local<v8::Value> DOMScriptingContext::createWindowProxy(v8::Local<v8::Context> context)
  {
    v8::Context::Scope contextScope(context);
    v8::EscapableHandleScope handleScope(isolate);

    v8::Local<v8::FunctionTemplate> windowProxyFunctionTemplate = v8::FunctionTemplate::New(isolate);
    v8::Local<v8::ObjectTemplate> windowProxyTemplate = windowProxyFunctionTemplate->InstanceTemplate();

    v8::NamedPropertyHandlerConfiguration namedConfig(
        WindowProxyPropertyGetterCallback,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        {},
        v8::PropertyHandlerFlags::kHasNoSideEffect);
    windowProxyTemplate->SetHandler(namedConfig);

    auto windowProxy = windowProxyTemplate->NewInstance(context).ToLocalChecked();
    return handleScope.Escape(windowProxy);
  }

  v8::Local<v8::Value> DOMScriptingContext::createWorkerSelfProxy(v8::Local<v8::Context> context)
  {
    v8::Context::Scope contextScope(context);
    v8::EscapableHandleScope handleScope(isolate);

    v8::Local<v8::FunctionTemplate> selfProxyFunctionTemplate = v8::FunctionTemplate::New(isolate);
    v8::Local<v8::ObjectTemplate> selfProxyTemplate = selfProxyFunctionTemplate->InstanceTemplate();

    v8::NamedPropertyHandlerConfiguration namedConfig(
        WorkerSelfProxyPropertyGetterCallback,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        {},
        v8::PropertyHandlerFlags::kHasNoSideEffect);
    selfProxyTemplate->SetHandler(namedConfig);

    auto selfProxy = selfProxyTemplate->NewInstance(context).ToLocalChecked();
    return handleScope.Escape(selfProxy);
  }

  DOMScript::DOMScript(SourceTextType sourceTextType, shared_ptr<RuntimeContext> runtimeContext)
      : sourceTextType(sourceTextType),
        runtimeContext(runtimeContext)
  {
    static TrIdGenerator scriptIdGen(0xf9);
    id = scriptIdGen.get();
  }

  DOMClassicScript::DOMClassicScript(shared_ptr<RuntimeContext> runtimeContext)
      : DOMScript(SourceTextType::Classic, runtimeContext)
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

  DOMModule::DOMModule(shared_ptr<RuntimeContext> runtimeContext)
      : DOMScript(SourceTextType::ESM, runtimeContext)
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

  string DOMModule::getUrlBySpecifier(const string &specifier)
  {
    /**
     * When specifier starts with "http:" or "https:", it is a URL string and should be used directly.
     */
    if (specifier.find("http:") == 0 || specifier.find("https:") == 0)
      return specifier;

    string nextSpecifier;
    auto scriptingContext = runtimeContext.lock()->scriptingContext;
    auto exactMatchUrl = scriptingContext->exactMatchImportMap(specifier);
    if (exactMatchUrl.has_value())
    {
      nextSpecifier = exactMatchUrl.value();
    }
    else
    {
      auto prefixMatchUrl = scriptingContext->prefixMatchImportMap(specifier);
      if (prefixMatchUrl.has_value())
        nextSpecifier = prefixMatchUrl.value();
    }

    if (nextSpecifier.empty())
      nextSpecifier = specifier;
    return crates::jsar::UrlHelper::CreateUrlStringWithPath(url, nextSpecifier);
  }

  void DOMModule::setLinkFinishedCallback(function<void(shared_ptr<DOMModule>)> callback)
  {
    linkFinishedCallback = callback;
  }

  void DOMModule::link(v8::Isolate *isolate)
  {
    v8::HandleScope scope(isolate);
    auto module = moduleStore.Get(isolate);
    auto v8Context = isolate->GetCurrentContext();
    auto scriptingContext = runtimeContext.lock()->scriptingContext;

    // Get module requests
    vector<ModuleRequestInfo> moduleRequestInfos;
    auto moduleRequests = module->GetModuleRequests();
    for (int i = 0; i < moduleRequests->Length(); i++)
    {
      auto moduleRequest = moduleRequests->Get(v8Context, i).As<v8::ModuleRequest>();
      auto specifier = moduleRequest->GetSpecifier();
      string specifier_utf8(*v8::String::Utf8Value(isolate, specifier));

      auto moduleUrl = getUrlBySpecifier(specifier_utf8);
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
        auto specifierRef = make_shared<string>(moduleRequestInfo.specifier); // reference to ensure lifetime when callback is called.
        scriptingContext->tryImportModule(moduleRequestInfo.url, false, [this, specifierRef](shared_ptr<DOMModule> module)
                                          { handleModuleLoaded(*specifierRef, module); });
      }
    }
  }

  template <typename T>
  inline void USE(T &&) {}

  bool DOMModule::instantiate(v8::Isolate *isolate)
  {
    v8::HandleScope scope(isolate);
    auto module = moduleStore.Get(isolate);
    auto context = isolate->GetCurrentContext();
    v8::TryCatch tryCatch(isolate);

    USE(module->InstantiateModule(context, ResolveModuleCallback));
    resolveCache.clear(); // clear the resolve cache after instantiation, which is used in module initialization.

    if (tryCatch.HasCaught() && !tryCatch.HasTerminated())
    {
      assert(tryCatch.Message().IsEmpty() != true);
      assert(tryCatch.Exception().IsEmpty() != true);
      auto stackTrace = tryCatch.StackTrace(context).ToLocalChecked();
      if (!stackTrace.IsEmpty())
      {
        auto stackTraceString = stackTrace->ToString(context).ToLocalChecked();
        v8::String::Utf8Value stackTraceUtf8(isolate, stackTraceString);
        std::cerr << "#" << std::endl;
        std::cerr << "# Occurred module instantiation error" << std::endl;
        std::cerr << "# URL: " << url << std::endl;
        std::cerr << "# Error:" << *stackTraceUtf8 << std::endl;
        std::cerr << "#" << std::endl;
      }
      return false;
    }
    return true;
  }

  void DOMModule::handleModuleLoaded(const string &specifier, shared_ptr<DOMModule> newModule)
  {
    assert(newModule != nullptr);
    resolveCache.insert({specifier, newModule});
    validModuleRequestsCount--;
    checkLinkFinished();
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
    {
      auto selfRef = dynamic_pointer_cast<DOMModule>(shared_from_this());
      linkFinishedCallback(selfRef);
    }
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
