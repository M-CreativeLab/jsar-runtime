#include <assert.h>
#include <rapidjson/document.h>
#include <idgen.hpp>
#include <crates/bindings.hpp>
#include <client/script_bindings/window.hpp>
#include <client/script_bindings/dom/document.hpp>
#include <client/script_bindings/binding.hpp>

#include "./dom_scripting.hpp"
#include "./runtime_context.hpp"

namespace dom
{
  using namespace std;
  using namespace v8;

  int const ContextEmbedderTag::kMyContextTag = 0x8e8f99;
  void *const ContextEmbedderTag::kMyContextTagPtr = const_cast<void *>(
    static_cast<const void *>(&ContextEmbedderTag::kMyContextTag));

  DOMScriptingContext *DOMScriptingContext::GetCurrent(Local<Context> context)
  {
    auto externalObject = context->GetEmbedderData(ContextEmbedderIndex::kScriptingContextExternal).As<External>();
    if (!externalObject.IsEmpty())
      return static_cast<DOMScriptingContext *>(externalObject->Value());
    else
      return nullptr;
  }

  void DOMScriptingContext::PropertyGetterCallback(Local<Name> property, const PropertyCallbackInfo<Value> &info)
  {
    auto isolate = info.GetIsolate();
    auto context = isolate->GetCurrentContext();
    auto sandbox = context->GetEmbedderData(ContextEmbedderIndex::kSandboxObject).As<Object>();

    MaybeLocal<Value> maybeValue = sandbox->GetRealNamedProperty(context, property);
    if (maybeValue.IsEmpty())
      maybeValue = context->Global()->GetRealNamedProperty(context, property);

    Local<Value> resultValue;
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

  void DOMScriptingContext::PropertySetterCallback(Local<Name> property,
                                                   Local<Value> value,
                                                   const PropertyCallbackInfo<Value> &info)
  {
    auto isolate = info.GetIsolate();
    auto context = isolate->GetCurrentContext();
    auto sandbox = context->GetEmbedderData(ContextEmbedderIndex::kSandboxObject).As<Object>();

    cout << "Setting property: " << *String::Utf8Value(isolate, property) << endl;
    if (sandbox->Set(context, property, value).IsNothing())
    {
      // Handle error case - property couldn't be set
      return;
    }
    info.GetReturnValue().Set(value);
  }

  void DOMScriptingContext::WorkerSelfProxyPropertyGetterCallback(Local<Name> property, const PropertyCallbackInfo<Value> &info)
  {
    auto isolate = info.GetIsolate();
    auto context = isolate->GetCurrentContext();
    auto globalObject = context->Global();

    MaybeLocal<Value> maybeValue = globalObject->Get(context, property);
    /**
     * TODO: add new an embedder data for the window-only properties.
     */

    Local<Value> resultValue;
    if (maybeValue.ToLocal(&resultValue))
      info.GetReturnValue().Set(resultValue);
    else
      info.GetReturnValue().SetUndefined();
  }

  MaybeLocal<Promise> DOMScriptingContext::ImportModuleDynamicallyCallback(Local<Context> context,
                                                                           Local<Data> hostDefinedOptions,
                                                                           Local<Value> resourceName,
                                                                           Local<String> specifier,
                                                                           Local<FixedArray> importAssertions)
  {
    auto isolate = context->GetIsolate();
    EscapableHandleScope handleScope(isolate);

    Local<FixedArray> options = hostDefinedOptions.As<FixedArray>();
    DOMScriptingContext *scriptingContext = DOMScriptingContext::GetCurrent(context);
    if (scriptingContext == nullptr)
    {
      cerr << "Failed to get the scripting context object" << endl;
      return MaybeLocal<Promise>();
    }

    String::Utf8Value specifier_utf8(isolate, specifier);
    string specifierStr(*specifier_utf8, specifier_utf8.length());

    auto type = options->Get(context, HostDefinedOptions::kType).As<Number>()->Int32Value(context).ToChecked();
    auto id = options->Get(context, HostDefinedOptions::kID).As<Number>()->Int32Value(context).ToChecked();

    if (type != (int)SourceTextType::ESM)
    {
      Local<Promise::Resolver> resolver;
      if (!Promise::Resolver::New(context).ToLocal(&resolver))
        return MaybeLocal<Promise>();

      auto typeError = Exception::TypeError(String::NewFromUtf8(isolate, "dynamic import() is only supported in ECMAScript module.").ToLocalChecked());
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
        cerr << "request for '" << specifierStr << "' is from invalid module" << endl;
        return MaybeLocal<Promise>();
      }

      auto moduleUrl = dependent->getUrlBySpecifier(specifierStr);
      if (moduleUrl == "")
      {
        cerr << "Failed to create URL for '" << specifierStr << "'" << endl;
        return MaybeLocal<Promise>();
      }

      Local<Promise::Resolver> resolver;
      if (!Promise::Resolver::New(context).ToLocal(&resolver))
        return MaybeLocal<Promise>();

      auto scriptingContext = dependent->runtimeContext.lock()->scriptingContext;
      auto persistentResolver = make_shared<Persistent<Promise::Resolver>>(isolate, resolver);
      auto specifierRef = make_shared<string>(specifierStr);
      auto onModuleLoaded = [isolate, persistentResolver, dependent, scriptingContext, specifierRef](shared_ptr<DOMModule> module)
      {
        Isolate::Scope isolateScope(isolate);
        HandleScope handleScope(isolate);

        auto resolver = persistentResolver->Get(isolate);
        dependent->resolveCache.insert({*specifierRef, module});
        scriptingContext->evaluate(dynamic_pointer_cast<DOMScript>(module));
        resolver->Resolve(isolate->GetCurrentContext(), module->getExports(isolate)).ToChecked();
        persistentResolver->Reset();
      };
      scriptingContext->tryImportModule(moduleUrl, false, onModuleLoaded);
      return handleScope.Escape(resolver->GetPromise());
    }
  }

  DOMScriptingContext::DOMScriptingContext(shared_ptr<RuntimeContext> runtimeContext)
      : isolate_(Isolate::GetCurrent())
      , runtimeContext(runtimeContext)
  {
    assert(isolate_ != nullptr && "Failed to get the current V8 isolate.");
  }

  void DOMScriptingContext::enableDynamicImport()
  {
    auto mainContext = isolate_->GetCurrentContext();
    Isolate::Scope isolateScope(isolate_);
    Context::Scope contextScope(mainContext);
    isolate_->SetHostImportModuleDynamicallyCallback(ImportModuleDynamicallyCallback);
  }

  void DOMScriptingContext::makeMainContext(shared_ptr<dom::Node> nativeDocument,
                                            shared_ptr<browser::Window> nativeWindow)
  {
    assert(!isContextInitialized);
    assert(v8ContextStore.IsEmpty());
    assert(nativeDocument != nullptr && nativeDocument->isHTMLDocument() &&
           "The document must be an HTML document.");

    auto mainContext = isolate_->GetCurrentContext();
    {
      Isolate::Scope isolateScope(isolate_);
      Context::Scope contextScope(mainContext);
      HandleScope handleScope(isolate_);

      // Initialize the Window firstly
      auto Window = script_bindings::Window::Initialize(isolate_);
      Local<Context> scriptingContext = Context::New(isolate_,
                                                     nullptr,
                                                     script_bindings::Window::GetInstanceTemplate(isolate_),
                                                     script_bindings::Window::GetOrNewInstance(isolate_, nativeWindow));
      {
        Context::Scope contextScope(scriptingContext);
        HandleScope handleScope(isolate_);

        // Initialize the scripting context from script bindings
        script_bindings::Initialize(isolate_, scriptingContext, script_bindings::ContextType::kScripting);

        // Set the `window` and `self` properties to refer to the global object itself
        auto global = scriptingContext->Global();
        global->Set(scriptingContext,
                    String::NewFromUtf8(isolate_, "Window").ToLocalChecked(),
                    Window)
          .Check();
        global->Set(scriptingContext,
                    String::NewFromUtf8(isolate_, "window").ToLocalChecked(),
                    global)
          .Check();
        global->Set(scriptingContext,
                    String::NewFromUtf8(isolate_, "self").ToLocalChecked(),
                    global)
          .Check();

        // Set the `document` property to refer to the document object
        auto document = script_bindings::dom_bindings::Document::NewInstance(
          isolate_, static_pointer_cast<dom::Document>(nativeDocument));
        global->Set(scriptingContext,
                    String::NewFromUtf8(isolate_, "document").ToLocalChecked(),
                    document)
          .Check();

#define V8_SET_GLOBAL_FROM_VALUE(name, value)                             \
  if (!global->Set(scriptingContext,                                      \
                   String::NewFromUtf8(isolate_, #name).ToLocalChecked(), \
                   value)                                                 \
         .FromMaybe(false))                                               \
    cerr << "Failed to set the global object(" << #name << ") for scripting v8::Context." << endl;

#define V8_TRY_SET_GLOBAL_FROM_VALUE(name, valueOrExpr)                                                        \
  try                                                                                                          \
  {                                                                                                            \
    V8_SET_GLOBAL_FROM_VALUE(name, valueOrExpr);                                                               \
  }                                                                                                            \
  catch (const exception &e)                                                                                   \
  {                                                                                                            \
    cerr << "Failed to set the global object(" << #name << ") for main context, reason: " << e.what() << endl; \
  }

#define V8_SET_GLOBAL_FROM_MAIN(name)                                                                                 \
  do                                                                                                                  \
  {                                                                                                                   \
    Local<Value> valueToSet;                                                                                          \
    auto maybeValue = mainContext->Global()->Get(mainContext, String::NewFromUtf8(isolate_, #name).ToLocalChecked()); \
    if (!maybeValue.IsEmpty() && maybeValue.ToLocal(&valueToSet))                                                     \
    {                                                                                                                 \
      V8_SET_GLOBAL_FROM_VALUE(name, valueToSet);                                                                     \
    }                                                                                                                 \
  } while (0)

        /**
         * Configure the global objects and functions for the DOM scripting.
         */

        // Baisc objects
        V8_SET_GLOBAL_FROM_MAIN(performance);
        V8_SET_GLOBAL_FROM_MAIN(console);

        // Basic constructors
        V8_SET_GLOBAL_FROM_MAIN(URL);
        V8_SET_GLOBAL_FROM_MAIN(Blob);
        V8_SET_GLOBAL_FROM_MAIN(FormData);
        V8_SET_GLOBAL_FROM_MAIN(XMLHttpRequest);
        V8_SET_GLOBAL_FROM_MAIN(WebSocket);
        V8_SET_GLOBAL_FROM_MAIN(EventSource);
        V8_SET_GLOBAL_FROM_MAIN(TextDecoder);
        V8_SET_GLOBAL_FROM_MAIN(Audio);
        V8_SET_GLOBAL_FROM_MAIN(Image);
        V8_SET_GLOBAL_FROM_MAIN(Worker);

        // Global functions
        V8_SET_GLOBAL_FROM_MAIN(atob);
        V8_SET_GLOBAL_FROM_MAIN(btoa);
        V8_TRY_SET_GLOBAL_FROM_VALUE(fetch, runtimeContext->createWHATWGFetchImpl(scriptingContext));
        V8_SET_GLOBAL_FROM_MAIN(setTimeout);
        V8_SET_GLOBAL_FROM_MAIN(clearTimeout);
        V8_SET_GLOBAL_FROM_MAIN(setInterval);
        V8_SET_GLOBAL_FROM_MAIN(clearInterval);
        V8_SET_GLOBAL_FROM_MAIN(requestAnimationFrame);
        V8_SET_GLOBAL_FROM_MAIN(cancelAnimationFrame);
        V8_SET_GLOBAL_FROM_MAIN(queueMicrotask);

        // Fetch API related objects
        V8_SET_GLOBAL_FROM_MAIN(Headers);
        V8_SET_GLOBAL_FROM_MAIN(Request);
        V8_SET_GLOBAL_FROM_MAIN(Response);

#undef V8_SET_GLOBAL_FROM_MAIN
#undef V8_TRY_SET_GLOBAL_FROM_VALUE
#undef V8_SET_GLOBAL_FROM_VALUE
      }

      ContextEmbedderTag::TagMyContext(scriptingContext);
      scriptingContext->SetEmbedderData(ContextEmbedderIndex::kScriptingContextExternal, External::New(isolate_, this));
      scriptingContext->SetSecurityToken(mainContext->GetSecurityToken());
      v8ContextStore.Reset(isolate_, scriptingContext);
    }
    isContextInitialized = true;
  }

  void DOMScriptingContext::makeWorkerContext()
  {
    assert(!isContextInitialized);
    assert(v8ContextStore.IsEmpty());
    auto mainContext = isolate_->GetCurrentContext();
    {
      Isolate::Scope isolateScope(isolate_);
      Context::Scope contextScope(mainContext);
      HandleScope handleScope(isolate_);

      Local<FunctionTemplate> globalFuncTemplate = FunctionTemplate::New(isolate_);
      Local<ObjectTemplate> globalObjectTemplate = globalFuncTemplate->InstanceTemplate();

      NamedPropertyHandlerConfiguration namedConfig(
        PropertyGetterCallback,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        {},
        PropertyHandlerFlags::kHasNoSideEffect);
      globalObjectTemplate->SetHandler(namedConfig);

      auto workerContext = Context::New(isolate_, nullptr, globalObjectTemplate);
      auto global = mainContext->Global();
      auto sandbox = Object::New(isolate_);
      {
#define V8_SET_GLOBAL_FROM_VALUE(name, value) \
  sandbox->Set(mainContext, String::NewFromUtf8(isolate_, #name).ToLocalChecked(), value).FromJust()
#define V8_TRY_SET_GLOBAL_FROM_VALUE(name, valueOrExpr)                                                          \
  try                                                                                                            \
  {                                                                                                              \
    V8_SET_GLOBAL_FROM_VALUE(name, valueOrExpr);                                                                 \
  }                                                                                                              \
  catch (const exception &e)                                                                                     \
  {                                                                                                              \
    cerr << "Failed to set the global object(" << #name << ") for worker context, reason: " << e.what() << endl; \
  }
#define V8_SET_GLOBAL_FROM_MAIN(name)                                                                  \
  do                                                                                                   \
  {                                                                                                    \
    Local<Value> valueToSet;                                                                           \
    auto maybeValue = global->Get(mainContext, String::NewFromUtf8(isolate_, #name).ToLocalChecked()); \
    if (!maybeValue.IsEmpty() && maybeValue.ToLocal(&valueToSet))                                      \
    {                                                                                                  \
      V8_SET_GLOBAL_FROM_VALUE(name, valueToSet);                                                      \
    }                                                                                                  \
  } while (0)

        /**
         * Configure the WorkerGlobalScope objects and functions.
         *
         * See https://developer.mozilla.org/en-US/docs/Web/API/WorkerGlobalScope
         */

        // Update context globals from script bindings
        // script_bindings::Initialize(isolate_, workerContext, script_bindings::ContextType::kWorker);

        // Baisc objects
        // Create custom console object with CDP integration using the Console binding
        // V8_SET_GLOBAL_FROM_VALUE(console, dombinding::Console::CreateV8Console(isolate_, workerContext));

        // Basic constructors
        V8_SET_GLOBAL_FROM_MAIN(URL);
        V8_SET_GLOBAL_FROM_MAIN(Blob);
        V8_SET_GLOBAL_FROM_MAIN(TextDecoder);
        V8_SET_GLOBAL_FROM_MAIN(OffscreenCanvas);
        V8_SET_GLOBAL_FROM_MAIN(Worker);

        // Global functions
        V8_SET_GLOBAL_FROM_MAIN(atob);
        V8_SET_GLOBAL_FROM_MAIN(btoa);
        V8_TRY_SET_GLOBAL_FROM_VALUE(fetch, runtimeContext->createWHATWGFetchImpl(workerContext));
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

      ContextEmbedderTag::TagMyContext(workerContext);
      workerContext->SetEmbedderData(ContextEmbedderIndex::kSandboxObject, sandbox);
      workerContext->SetEmbedderData(ContextEmbedderIndex::kScriptingContextExternal,
                                     External::New(isolate_, this));
      workerContext->SetSecurityToken(mainContext->GetSecurityToken());
      v8ContextStore.Reset(isolate_, workerContext);
    }

    {
      auto newContext = v8ContextStore.Get(isolate_);
      Context::Scope contextScope(newContext);
      HandleScope handleScope(isolate_);
      Local<Value> selfProxy = createWorkerSelfProxy(newContext);
      auto global = newContext->Global();
      global->Set(newContext, String::NewFromUtf8(isolate_, "self").ToLocalChecked(), selfProxy).FromJust();
    }
    isContextInitialized = true;
  }

  shared_ptr<DOMScript> DOMScriptingContext::create(shared_ptr<RuntimeContext> runtimeContext,
                                                    const string &url,
                                                    SourceTextType type)
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

  bool DOMScriptingContext::compile(shared_ptr<DOMScript> script, const string &source, bool isTypeScript)
  {
    assert(isContextInitialized);

    // Create a handle scope to store the context, that is a `Local<Context>`.
    Isolate::Scope isolateScope(isolate_);
    HandleScope handleScope(isolate_);

    Local<Context> context = v8ContextStore.Get(isolate_);
    Context::Scope contextScope(context);

    if (!script->compile(isolate_, source, isTypeScript))
      return false;

    // If the script is ESM, add it to the module map
    if (script->sourceTextType == SourceTextType::ESM)
    {
      auto module = dynamic_pointer_cast<DOMModule>(script);
      hashToModuleMap.insert({module->getModuleHash(), module});
      module->link(isolate_);
    }
    return true;
  }

  MaybeLocal<Function> DOMScriptingContext::compileFunction(const string &funcSource)
  {
    assert(isContextInitialized);

    // Create a handle scope to store the context, that is a `Local<Context>`.
    EscapableHandleScope handleScope(isolate_);

    Local<Context> context = v8ContextStore.Get(isolate_);
    Context::Scope contextScope(context);

    // Wrap the handler code in a function that accepts an event parameter
    string functionCode = "(function(event) { " + funcSource + " })";

    TryCatch tryCatch(isolate_);
    Local<String> source = String::NewFromUtf8(isolate_, functionCode.c_str()).ToLocalChecked();

    // Compile the function expression
    Local<Script> script;
    if (!Script::Compile(context, source).ToLocal(&script))
    {
      if (tryCatch.HasCaught())
      {
        String::Utf8Value exception(isolate_, tryCatch.Exception());
        cerr << "Failed to compile event handler: " << *exception << endl;
      }
      return MaybeLocal<Function>();
    }

    // Execute the script to get the function
    Local<Value> result;
    if (!script->Run(context).ToLocal(&result))
    {
      if (tryCatch.HasCaught())
      {
        String::Utf8Value exception(isolate_, tryCatch.Exception());
        cerr << "Failed to execute event handler compilation: " << *exception << endl;
      }
      return MaybeLocal<Function>();
    }

    // Ensure the result is a function
    if (!result->IsFunction())
    {
      cerr << "Event handler compilation did not result in a function" << endl;
      return MaybeLocal<Function>();
    }

    return handleScope.Escape(result.As<Function>());
  }

  bool DOMScriptingContext::compileAsSyntheticModule(shared_ptr<DOMModule> module, SyntheticModuleType type, const void *sourceData, size_t sourceByteLength)
  {
    assert(isContextInitialized);
    Isolate::Scope isolateScope(isolate_);
    HandleScope handleScope(isolate_);

    auto context = v8ContextStore.Get(isolate_);
    Context::Scope contextScope(context);

    if (!module->compileAsSyntheticModule(isolate_, type, sourceData, sourceByteLength))
      return false;

    hashToModuleMap.insert({module->getModuleHash(), module});
    module->link(isolate_);
    return true;
  }

  void DOMScriptingContext::evaluate(shared_ptr<DOMScript> script)
  {
    assert(isContextInitialized);
    Isolate::Scope isolateScope(isolate_);
    HandleScope handleScope(isolate_);

    Local<Context> context = v8ContextStore.Get(isolate_);
    Context::Scope contextScope(context);
    {
      script->evaluate(isolate_);
    }
  }

  shared_ptr<DOMModule> DOMScriptingContext::getModuleFromV8(Local<Module> module)
  {
    auto range = hashToModuleMap.equal_range(module->GetIdentityHash());
    for (auto it = range.first; it != range.second; ++it)
    {
      if (it->second->moduleStore.Get(isolate_) == module)
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

  void DOMScriptingContext::tryImportModule(const string &url,
                                            const bool disableCache,
                                            function<void(shared_ptr<DOMModule>)> loadedCallback,
                                            function<void(const string &)> errorCallback)
  {
    if (!disableCache)
    {
      auto module = getModuleFromUrl(url);
      if (module)
      {
        module->registerLinkedCallback(loadedCallback);
        return;
      }
    }
    shared_ptr<DOMScript> script = create(runtimeContext, url, SourceTextType::ESM);
    auto onModuleSourceLoaded = [this, script, loadedCallback](const void *sourceData, size_t sourceByteLength)
    {
      auto module = dynamic_pointer_cast<DOMModule>(script);
      module->registerLinkedCallback(loadedCallback, false /** Do not check for linked */);

      auto extension = crates::UrlHelper::ParseUrlToModuleExtension(module->url);
      if (extension.isTextSourceModule())
      {
        string source(static_cast<const char *>(sourceData), sourceByteLength);
        compile(script, source, extension.isTypeScript());
      }
      else if (
        extension.isBinary() ||
        extension.isWebAssembly() ||
        extension.isAudio() ||
        extension.isImage())
      {
        compileAsSyntheticModule(module, SyntheticModuleType::ArrayBuffer, sourceData, sourceByteLength);
      }
      else if (extension.isJson())
      {
        compileAsSyntheticModule(module, SyntheticModuleType::JSON, sourceData, sourceByteLength);
      }
      else
      {
        cerr << "Failed to load the module: " << module->url << ", the extension is not supported." << endl;
      }
    };
    runtimeContext->fetchArrayBufferLikeResource(url, onModuleSourceLoaded);
  }

  bool DOMScriptingContext::dispatchEvent(Local<Object> event)
  {
    assert(isContextInitialized);
    assert(!v8ContextStore.IsEmpty());

    auto context = v8ContextStore.Get(isolate_);
    Isolate::Scope isolateScope(isolate_);
    Context::Scope contextScope(context);

    auto eventType = event->Get(context, String::NewFromUtf8(isolate_, "type").ToLocalChecked()).ToLocalChecked();
    auto eventType_utf8 = String::Utf8Value(isolate_, eventType);

    auto global = context->Global();
    auto listenerName = "on" + string(*eventType_utf8, eventType_utf8.length());

    auto eventListener = global->Get(context, String::NewFromUtf8(isolate_, listenerName.c_str()).ToLocalChecked());
    if (!eventListener.IsEmpty() && eventListener.ToLocalChecked()->IsFunction())
    {
      TryCatch tryCatch(isolate_);
      auto function = eventListener.ToLocalChecked().As<Function>();
      Local<Value> argv[] = {event};
      Local<Value> result;
      if (!function->Call(context, global, 1, argv).ToLocal(&result))
      {
        if (tryCatch.HasCaught())
        {
          Local<Message> message = tryCatch.Message();
          String::Utf8Value messageUtf8(isolate_, message->Get());
          cerr << "Failed to call the event listener: " << listenerName << ", ";
          cerr << "occurred an error: " << *messageUtf8 << endl;
        }
        else
        {
          cerr << "Failed to call the event listener: " << listenerName << endl;
        }
      }
    }
    // TODO: call dispatchEvent?
    return true;
  }

  Local<Value> DOMScriptingContext::createWorkerSelfProxy(Local<Context> context)
  {
    Context::Scope contextScope(context);
    EscapableHandleScope handleScope(isolate_);

    Local<FunctionTemplate> selfProxyFunctionTemplate = FunctionTemplate::New(isolate_);
    Local<ObjectTemplate> selfProxyTemplate = selfProxyFunctionTemplate->InstanceTemplate();

    NamedPropertyHandlerConfiguration namedConfig(
      WorkerSelfProxyPropertyGetterCallback,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      {},
      PropertyHandlerFlags::kHasNoSideEffect);
    selfProxyTemplate->SetHandler(namedConfig);

    auto selfProxy = selfProxyTemplate->NewInstance(context).ToLocalChecked();
    return handleScope.Escape(selfProxy);
  }

  DOMScript::DOMScript(SourceTextType sourceTextType, shared_ptr<RuntimeContext> runtimeContext)
      : sourceTextType(sourceTextType)
      , runtimeContext(runtimeContext)
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

  bool DOMClassicScript::compile(Isolate *isolate, const string &sourceStr, bool isTypeScript)
  {
    HandleScope scope(isolate);

    // create the script origin
    auto urlString = String::NewFromUtf8(isolate, url.c_str()).ToLocalChecked();
    int lineOffset = 0;
    int columnOffset = 0;

    Local<PrimitiveArray> hostDefinedOptions = PrimitiveArray::New(isolate, HostDefinedOptions::kLength);
    hostDefinedOptions->Set(isolate, HostDefinedOptions::kType, Number::New(isolate, (int)SourceTextType::Classic));
    hostDefinedOptions->Set(isolate, HostDefinedOptions::kID, Number::New(isolate, id));

    ScriptOrigin origin(isolate,
                        urlString,
                        lineOffset,
                        columnOffset,
                        crossOrigin,
                        id,
                        Local<Value>(),
                        false,
                        false,
                        false,
                        hostDefinedOptions);

    // Transpile the TypeScript source code if necessary.
    string scriptSourceString;
    if (isTypeScript)
    {
      try
      {
        scriptSourceString = crates::TypeScriptTranspiler::Transpile(sourceStr);
      }
      catch (const exception &e)
      {
        cerr << "Failed to compile TypeScript: " << e.what() << endl;
        return false;
      }
    }
    else
    {
      scriptSourceString = sourceStr;
    }

    // create the script
    auto sourceString = String::NewFromUtf8(isolate, scriptSourceString.c_str()).ToLocalChecked();
    ScriptCompiler::Source source(sourceString, origin);

    // compile the script
    auto context = isolate->GetCurrentContext();
    auto maybeScript = ScriptCompiler::Compile(context, &source, ScriptCompiler::kNoCompileOptions);

    Local<Script> script;
    if (maybeScript.ToLocal(&script))
    {
      scriptStore.Reset(isolate, script);
      return true;
    }
    else
    {
      cerr << "Failed to compile script" << endl;
      cerr << "  URL: " << url << endl;
      cerr << "  Source: " << endl;
      cerr << scriptSourceString << endl;
      throw runtime_error("Failed to compile script(" + url + ")");
    }
  }

  void DOMClassicScript::evaluate(Isolate *isolate)
  {
    HandleScope handleScope(isolate);

    Local<Script> script = scriptStore.Get(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    TryCatch tryCatch(isolate);
    if (script->Run(context).IsEmpty())
    {
      cerr << "#" << endl;
      cerr << "# Failed to execute script" << endl;
      cerr << "# URL: " << url << endl;
      cerr << "#" << endl;

      Local<Message> message = tryCatch.Message();
      if (!message.IsEmpty())
      {
        String::Utf8Value messageUtf8(isolate, message->Get());
        string messageStr(*messageUtf8, messageUtf8.length());
        cerr << "# Error: " << messageStr << endl;

        Local<StackTrace> stacktrace = message->GetStackTrace();
        if (!stacktrace.IsEmpty())
        {
          for (int i = 0; i < stacktrace->GetFrameCount(); i++)
          {
            Local<StackFrame> frame = stacktrace->GetFrame(isolate, i);
            String::Utf8Value scriptName(isolate, frame->GetScriptName());
            String::Utf8Value functionName(isolate, frame->GetFunctionName());
            int lineNumber = frame->GetLineNumber();
            int column = frame->GetColumn();
            cerr << "#   at "
                 << *functionName << " (" << *scriptName << ":" << lineNumber << ":" << column << ")" << endl;
          }
        }
      }
    }
  }

  MaybeLocal<Module> DOMModule::ResolveModuleCallback(Local<Context> context,
                                                      Local<String> specifier,
                                                      Local<FixedArray> importAssertions,
                                                      Local<Module> referrer)
  {
    Isolate *isolate = context->GetIsolate();

    auto specifier_utf8 = String::Utf8Value(isolate, specifier);
    string specifierStr(*specifier_utf8, specifier_utf8.length());

    DOMScriptingContext *scriptingContext = DOMScriptingContext::GetCurrent(context);
    if (scriptingContext == nullptr)
    {
      cerr << "request for '" << specifierStr << "' failed, scripting context is valid" << endl;
      return MaybeLocal<Module>();
    }

    auto dependent = scriptingContext->getModuleFromV8(referrer);
    if (dependent == nullptr)
    {
      cerr << "request for '" << specifierStr << "' is from invalid module" << endl;
      return MaybeLocal<Module>();
    }
    if (dependent->resolveCache.count(specifierStr) != 1)
    {
      cerr << "request for '" << specifierStr << "' is not in cache" << endl;
      return MaybeLocal<Module>();
    }

    auto module = dependent->resolveCache[specifierStr];
    auto res = module->moduleStore.Get(isolate);
    return res;
  }

  MaybeLocal<Value> DOMModule::SyntheticModuleEvaluationStepsCallback(Local<Context> context,
                                                                      Local<Module> v8module)
  {
    Isolate *isolate = context->GetIsolate();
    HandleScope handleScope(isolate);

    DOMScriptingContext *scriptingContext = DOMScriptingContext::GetCurrent(context);
    if (scriptingContext == nullptr)
      return MaybeLocal<Value>();

    shared_ptr<DOMModule> module = scriptingContext->getModuleFromV8(v8module);
    if (module == nullptr)
      return MaybeLocal<Value>();

    Local<Promise::Resolver> resolver;
    if (!Promise::Resolver::New(context).ToLocal(&resolver))
      return MaybeLocal<Value>();

    auto defaultExportName = String::NewFromUtf8(isolate, "default").ToLocalChecked();
    USE(v8module->SetSyntheticModuleExport(isolate, defaultExportName, module->getExports(isolate)));

    resolver->Resolve(context, Undefined(isolate)).ToChecked();
    return resolver->GetPromise();
  }

  DOMModule::DOMModule(shared_ptr<RuntimeContext> runtimeContext)
      : DOMScript(SourceTextType::ESM, runtimeContext)
  {
  }

  DOMModule::~DOMModule()
  {
    moduleStore.Reset();
  }

  bool DOMModule::compile(Isolate *isolate, const string &sourceStr, bool isTypeScript)
  {
    HandleScope scope(isolate);

    // create the script origin
    auto urlString = String::NewFromUtf8(isolate, url.c_str()).ToLocalChecked();
    int lineOffset = 0;
    int columnOffset = 0;

    Local<PrimitiveArray> hostDefinedOptions = PrimitiveArray::New(isolate, HostDefinedOptions::kLength);
    hostDefinedOptions->Set(isolate, HostDefinedOptions::kType, Number::New(isolate, (int)SourceTextType::ESM));
    hostDefinedOptions->Set(isolate, HostDefinedOptions::kID, Number::New(isolate, id));

    ScriptOrigin origin(isolate,
                        urlString,
                        lineOffset,
                        columnOffset,
                        crossOrigin,
                        id,
                        Local<Value>(),
                        false,
                        false,
                        true,
                        hostDefinedOptions);

    // Transpile the TypeScript source code if necessary.
    string scriptSourceString;
    if (isTypeScript)
    {
      try
      {
        scriptSourceString = crates::TypeScriptTranspiler::Transpile(sourceStr);
      }
      catch (const exception &e)
      {
        cerr << "Failed to compile TypeScript: " << e.what() << endl;
        return false;
      }
    }
    else
    {
      scriptSourceString = sourceStr;
    }

    // create the script
    auto sourceString = String::NewFromUtf8(isolate, scriptSourceString.c_str()).ToLocalChecked();
    ScriptCompiler::Source source(sourceString, origin);
    ScriptCompiler::CompileOptions options = ScriptCompiler::kNoCompileOptions;

    // compile the script
    Local<Module> module;
    auto context = isolate->GetCurrentContext();

    if (!ScriptCompiler::CompileModule(isolate, &source, options).ToLocal(&module))
    {
      cerr << "#" << endl;
      cerr << "# Occurred module compilation error" << endl;
      cerr << "# URL: " << url << endl;
      cerr << "#" << endl;
      cerr << sourceStr << endl;
      return false;
    }
    else
    {
      moduleStore.Reset(isolate, module);
      return true;
    }
  }

  bool DOMModule::compileAsSyntheticModule(Isolate *isolate, SyntheticModuleType type, const void *sourceData, size_t sourceByteLength)
  {
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    TryCatch tryCatch(isolate);
    Local<String> urlString = String::NewFromUtf8(isolate, url.c_str()).ToLocalChecked();
    Local<String> defaultExportName = String::NewFromUtf8(isolate, "default").ToLocalChecked();

    switch (type)
    {
    case SyntheticModuleType::JSON:
    {
      Local<Value> sourceJsonObject;
      Local<String> sourceString = String::NewFromUtf8(isolate, static_cast<const char *>(sourceData), NewStringType::kNormal, sourceByteLength).ToLocalChecked();
      if (JSON::Parse(context, sourceString).ToLocal(&sourceJsonObject))
        syntheticModuleNamespaceStore.Reset(isolate, sourceJsonObject);
      break;
    }
    case SyntheticModuleType::ArrayBuffer:
    {
      Local<ArrayBuffer> sourceArrayBuffer = ArrayBuffer::New(isolate, sourceByteLength);
      memcpy(sourceArrayBuffer->GetBackingStore()->Data(), sourceData, sourceByteLength);
      syntheticModuleNamespaceStore.Reset(isolate, sourceArrayBuffer.As<Value>());
      break;
    }
    default:
      break;
    }

    if (tryCatch.HasCaught())
    {
      cerr << "#" << endl;
      cerr << "# Occurred synthetic module compilation error" << endl;
      cerr << "# URL: " << url << endl;
      {
        // Print the exception message
        Local<Message> message = tryCatch.Message();
        String::Utf8Value messageUtf8(isolate, message->Get());
        string messageStr(*messageUtf8, messageUtf8.length());
        cerr << "# Error: " << messageStr << endl;
      }
      cerr << "#" << endl;
      return false;
    }

    Local<Module> module = Module::CreateSyntheticModule(isolate,
                                                         urlString,
                                                         {defaultExportName},
                                                         SyntheticModuleEvaluationStepsCallback);
    moduleStore.Reset(isolate, module);
    return true;
  }

  void DOMModule::evaluate(Isolate *isolate)
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
    auto module = moduleStore.Get(Isolate::GetCurrent());
    return module->GetIdentityHash();
  }

  string DOMModule::getUrlBySpecifier(const string &specifier)
  {
    /**
     * When specifier starts with "http:" or "https:", it is a URL string and should be used directly.
     */
    if (specifier.find("http:") == 0 || specifier.find("https:") == 0)
      return specifier;

    /**
     * TODO: support jsr: scheme
     * @see https://jsr.io/docs/api#jsr-registry-api
     */

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
    return crates::UrlHelper::CreateUrlStringWithPath(url, nextSpecifier);
  }

  void DOMModule::registerLinkedCallback(ModuleLinkedCallback callback, bool checkLinked)
  {
    if (checkLinked && linked)
      callback(dynamic_pointer_cast<DOMModule>(shared_from_this()));
    else
      linkedCallbacks.push_back(callback);
  }

  Local<Value> DOMModule::getExports(Isolate *isolate)
  {
    EscapableHandleScope handleScope(isolate);
    Local<Module> module = moduleStore.Get(isolate);
    if (module.IsEmpty())
      return handleScope.Escape(Undefined(isolate));

    if (module->IsSyntheticModule())
    {
      if (syntheticModuleNamespaceStore.IsEmpty())
        return handleScope.Escape(Undefined(isolate));
      else
        return handleScope.Escape(syntheticModuleNamespaceStore.Get(isolate));
    }
    else
    {
      auto modNamespace = module->GetModuleNamespace();
      if (modNamespace.IsEmpty())
        return handleScope.Escape(Undefined(isolate));
      else
        return handleScope.Escape(modNamespace);
    }
  }

  void DOMModule::link(Isolate *isolate)
  {
    HandleScope scope(isolate);
    auto module = moduleStore.Get(isolate);

    if (module->IsSyntheticModule()) // Synthetic module should not be linked
    {
      onLinkFinished();
      return;
    }

    auto v8Context = isolate->GetCurrentContext();
    auto scriptingContext = runtimeContext.lock()->scriptingContext;

    // Get module requests
    vector<ModuleRequestInfo> moduleRequestInfos;
    auto moduleRequests = module->GetModuleRequests();
    for (int i = 0; i < moduleRequests->Length(); i++)
    {
      auto moduleRequest = moduleRequests->Get(v8Context, i).As<ModuleRequest>();
      auto specifier = moduleRequest->GetSpecifier();
      string specifier_utf8(*String::Utf8Value(isolate, specifier));

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

  bool DOMModule::instantiate(Isolate *isolate)
  {
    HandleScope scope(isolate);
    Local<Module> module = moduleStore.Get(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    TryCatch tryCatch(isolate);

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
        String::Utf8Value stackTraceUtf8(isolate, stackTraceString);
        cerr << "#" << endl;
        cerr << "# Occurred module instantiation error" << endl;
        cerr << "# URL: " << url << endl;
        cerr << "# Error:" << *stackTraceUtf8 << endl;
        cerr << "#" << endl;
      }
      return false;
    }
    else
    {
      return true;
    }
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
    if (TR_UNLIKELY(linked))
      return;

    Isolate *isolate = Isolate::GetCurrent();
    instantiate(isolate);

    linked = true;
    if (linkedCallbacks.size() > 0)
    {
      auto selfRef = dynamic_pointer_cast<DOMModule>(shared_from_this());
      for (auto callback : linkedCallbacks)
        callback(selfRef);
      linkedCallbacks.clear();
    }

    if (evaluationScheduled)
      doEvaluate(isolate);
  }

  void DOMModule::doEvaluate(Isolate *isolate)
  {
    if (evaluatedOnce)
      return;
    evaluatedOnce = true;

    HandleScope scope(isolate);
    auto module = moduleStore.Get(isolate);
    auto context = isolate->GetCurrentContext();

    if (module->IsSyntheticModule()) // Synthetic module should not be evaluated
      return;
    if (module->GetStatus() != Module::Status::kInstantiated)
    {
      cerr << "Failed to evaluate the module: " << url << ", the module is not instantiated." << endl;
      return;
    }

    Local<Value> resultValue;
    {
      TryCatch tryCatch(isolate);
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
        if (!resultValue->IsPromise())
        {
          cerr << "Failed to execute script: the result is not a promise" << endl;
        }
        else
        {
          auto promise = Local<Promise>::Cast(resultValue);
          auto resolveCallback = [](const FunctionCallbackInfo<Value> &info)
          {
            Isolate *isolate = info.GetIsolate();
            Local<Context> context = isolate->GetCurrentContext();
            Context::Scope contextScope(context);
            HandleScope handleScope(isolate);
            assert(info.Length() > 0);
            info.GetReturnValue().Set(info[0]);
          };
          auto rejectCallback = [](const FunctionCallbackInfo<Value> &info)
          {
            Isolate *isolate = info.GetIsolate();
            Local<Context> context = isolate->GetCurrentContext();
            Context::Scope contextScope(context);
            HandleScope handleScope(isolate);
            String::Utf8Value message(info.GetIsolate(), info[0]);
            cerr << "Failed to execute script: " << *message << endl;
          };
          Local<Function> resolveFunc = Function::New(context, resolveCallback).ToLocalChecked();
          Local<Function> rejectFunc = Function::New(context, rejectCallback).ToLocalChecked();
        }
      }
    }
  }
}
