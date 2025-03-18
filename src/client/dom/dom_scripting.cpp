#include <assert.h>
#include <rapidjson/document.h>
#include <idgen.hpp>
#include <crates/bindings.hpp>

#include "./dom_scripting.hpp"
#include "./runtime_context.hpp"

namespace dom
{
  using namespace std;

  int const ContextEmbedderTag::kMyContextTag = 0x8e8f99;
  void *const ContextEmbedderTag::kMyContextTagPtr = const_cast<void *>(
      static_cast<const void *>(&ContextEmbedderTag::kMyContextTag));

  DOMScriptingContext *DOMScriptingContext::GetCurrent(v8::Local<v8::Context> context)
  {
    auto externalObject = context->GetEmbedderData(ContextEmbedderIndex::kScriptingContextExternal).As<v8::External>();
    if (!externalObject.IsEmpty())
      return static_cast<DOMScriptingContext *>(externalObject->Value());
    else
      return nullptr;
  }

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
    auto internalValue = context->GetEmbedderData(ContextEmbedderIndex::kInternalObject);

    /**
     * 1. Check if the property is in the window object.
     */
    if (!internalValue.IsEmpty() && internalValue->IsObject())
    {
      auto internalObject = internalValue.As<v8::Object>();
      auto windowKey = v8::String::NewFromUtf8(isolate, "window").ToLocalChecked();
      auto windowValue = internalObject->GetRealNamedProperty(context, windowKey).ToLocalChecked();
      auto windowObject = windowValue.As<v8::Object>();
      if (windowObject->Has(context, property).ToChecked())
      {
        v8::MaybeLocal<v8::Value> maybeValue = windowObject->Get(context, property);
        v8::Local<v8::Value> resultValue;
        if (maybeValue.ToLocal(&resultValue))
        {
          /**
           * If the property is a function, we need to return a bound function with the window object to avoid the scope issue.
           */
          if (resultValue->IsFunction())
          {
            auto func = resultValue.As<v8::Function>();
            auto bind = func->Get(context, v8::String::NewFromUtf8(isolate, "bind").ToLocalChecked()).ToLocalChecked().As<v8::Function>();
            resultValue = bind->Call(context, func, 1, &windowValue).ToLocalChecked();
          }
          return info.GetReturnValue().Set(resultValue);
        }
      }
    }

    /**
     * 2. Check if the property is in the global object.
     */
    if (globalObject->Has(context, property).ToChecked())
    {
      v8::MaybeLocal<v8::Value> maybeValue = globalObject->Get(context, property);
      v8::Local<v8::Value> resultValue;
      if (maybeValue.ToLocal(&resultValue))
        return info.GetReturnValue().Set(resultValue);
    }

    /**
     * 3. Otherwise, namely the property is not found from the window and global objects, returns undefined.
     */
    info.GetReturnValue().SetUndefined();
  }

  void DOMScriptingContext::WindowProxyPropertyEnumeratorCallback(const v8::PropertyCallbackInfo<v8::Array> &info)
  {
    auto isolate = info.GetIsolate();
    auto context = isolate->GetCurrentContext();
    auto globalObject = context->Global();
    v8::HandleScope handleScope(isolate);

    uint32_t propertyIndex = 0;
    v8::Local<v8::Array> resultArray = v8::Array::New(isolate);
    {
      auto globalPropertyNames = globalObject->GetPropertyNames(context).ToLocalChecked();
      auto globalLength = globalPropertyNames->Length();
      for (uint32_t propertyIndex = 0; propertyIndex < globalLength; propertyIndex++)
      {
        auto propertyName = globalPropertyNames->Get(context, propertyIndex).ToLocalChecked();
        resultArray->Set(context, propertyIndex, propertyName).ToChecked();
      }
    }
    info.GetReturnValue().Set(resultArray);
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
    DOMScriptingContext *scriptingContext = DOMScriptingContext::GetCurrent(context);
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
        resolver->Resolve(isolate->GetCurrentContext(), module->getExports(isolate)).ToChecked();
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

  void DOMScriptingContext::makeMainContext(v8::Local<v8::Value> windowValue, v8::Local<v8::Value> documentValue)
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
#define V8_TRY_SET_GLOBAL_FROM_VALUE(name, valueOrExpr)                                                                  \
  try                                                                                                                    \
  {                                                                                                                      \
    V8_SET_GLOBAL_FROM_VALUE(name, valueOrExpr);                                                                         \
  }                                                                                                                      \
  catch (const std::exception &e)                                                                                        \
  {                                                                                                                      \
    std::cerr << "Failed to set the global object(" << #name << ") for main context, reason: " << e.what() << std::endl; \
  }
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
        V8_SET_GLOBAL_FROM_MAIN(location);
        V8_SET_GLOBAL_FROM_MAIN(console);
        V8_SET_GLOBAL_FROM_MAIN(performance);

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

        // DOM nodes
        V8_SET_GLOBAL_FROM_MAIN(Node);
        V8_SET_GLOBAL_FROM_MAIN(Element);
        V8_SET_GLOBAL_FROM_MAIN(DocumentFragment);
        V8_SET_GLOBAL_FROM_MAIN(HTMLAnchorElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLAreaElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLAudioElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLBaseElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLBodyElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLBRElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLButtonElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLCanvasElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLDataElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLDataListElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLDetailsElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLDialogElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLDivElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLDListElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLEmbedElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLFieldSetElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLFormElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLHeadElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLHeadingElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLHRElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLHtmlElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLIFrameElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLImageElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLInputElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLStyleElement);
        V8_SET_GLOBAL_FROM_MAIN(HTMLTemplateElement);
        V8_SET_GLOBAL_FROM_MAIN(CharacterData);
        V8_SET_GLOBAL_FROM_MAIN(Text);

        // DOM APIs
        V8_SET_GLOBAL_FROM_MAIN(MutationRecord);
        V8_SET_GLOBAL_FROM_MAIN(MutationObserver);

        // Events
        V8_SET_GLOBAL_FROM_MAIN(Event);
        V8_SET_GLOBAL_FROM_MAIN(EventTarget);
        V8_SET_GLOBAL_FROM_MAIN(AnimationEvent);
        V8_SET_GLOBAL_FROM_MAIN(BlobEvent);
        V8_SET_GLOBAL_FROM_MAIN(CloseEvent);
        V8_SET_GLOBAL_FROM_MAIN(CompositionEvent);
        V8_SET_GLOBAL_FROM_MAIN(CustomEvent);
        V8_SET_GLOBAL_FROM_MAIN(ErrorEvent);
        V8_SET_GLOBAL_FROM_MAIN(FocusEvent);
        V8_SET_GLOBAL_FROM_MAIN(FormDataEvent);
        V8_SET_GLOBAL_FROM_MAIN(HashChangeEvent);
        V8_SET_GLOBAL_FROM_MAIN(InputEvent);
        V8_SET_GLOBAL_FROM_MAIN(KeyboardEvent);
        V8_SET_GLOBAL_FROM_MAIN(MessageEvent);
        V8_SET_GLOBAL_FROM_MAIN(MouseEvent);
        V8_SET_GLOBAL_FROM_MAIN(PointerEvent);
        V8_SET_GLOBAL_FROM_MAIN(ProgressEvent);
        V8_SET_GLOBAL_FROM_MAIN(TouchEvent);
        V8_SET_GLOBAL_FROM_MAIN(UIEvent);
        V8_SET_GLOBAL_FROM_MAIN(WebGLContextEvent);
        V8_SET_GLOBAL_FROM_MAIN(WheelEvent);

        // Global functions
        V8_SET_GLOBAL_FROM_MAIN(atob);
        V8_SET_GLOBAL_FROM_MAIN(btoa);
        V8_TRY_SET_GLOBAL_FROM_VALUE(fetch, runtimeContext->createWHATWGFetchImpl(newContext));
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

        // Canvas API
        V8_SET_GLOBAL_FROM_MAIN(OffscreenCanvas);
        V8_SET_GLOBAL_FROM_MAIN(OffscreenCanvasRenderingContext2D);
        V8_SET_GLOBAL_FROM_MAIN(CanvasRenderingContext2D);
        V8_SET_GLOBAL_FROM_MAIN(Path2D);

        // WebGL objects
        V8_SET_GLOBAL_FROM_MAIN(WebGLRenderingContext);
        V8_SET_GLOBAL_FROM_MAIN(WebGL2RenderingContext);

        // WebXR Device API
        V8_SET_GLOBAL_FROM_MAIN(XRSystem);
        V8_SET_GLOBAL_FROM_MAIN(XRSession);
        V8_SET_GLOBAL_FROM_MAIN(XRFrame);
        V8_SET_GLOBAL_FROM_MAIN(XRReferenceSpace);
        V8_SET_GLOBAL_FROM_MAIN(XRRigidTransform);
        V8_SET_GLOBAL_FROM_MAIN(XRWebGLLayer);

        /**
         * Re-exposing the following types is to avoid the "instanceof" issues between v8 contexts.
         */
        // Typed arrays
        V8_SET_GLOBAL_FROM_MAIN(Array);
        V8_SET_GLOBAL_FROM_MAIN(ArrayBuffer);
        V8_SET_GLOBAL_FROM_MAIN(SharedArrayBuffer);
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
#undef V8_TRY_SET_GLOBAL_FROM_VALUE
#undef V8_SET_GLOBAL_FROM_VALUE
      }

      auto internal = v8::Object::New(isolate);
      {
        // Internal object is used to store internally.
#define V8_SET_GLOBAL_FROM_VALUE(name, value) \
  internal->Set(mainContext, v8::String::NewFromUtf8(isolate, #name).ToLocalChecked(), value).FromJust()

        V8_SET_GLOBAL_FROM_VALUE(window, windowValue);
#undef V8_SET_GLOBAL_FROM_VALUE
      }

      ContextEmbedderTag::TagMyContext(newContext);
      newContext->SetEmbedderData(ContextEmbedderIndex::kSandboxObject, sandbox);
      newContext->SetEmbedderData(ContextEmbedderIndex::kInternalObject, internal);
      newContext->SetEmbedderData(ContextEmbedderIndex::kScriptingContextExternal, v8::External::New(isolate, this));
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
#define V8_TRY_SET_GLOBAL_FROM_VALUE(name, valueOrExpr)                                                                    \
  try                                                                                                                      \
  {                                                                                                                        \
    V8_SET_GLOBAL_FROM_VALUE(name, valueOrExpr);                                                                           \
  }                                                                                                                        \
  catch (const std::exception &e)                                                                                          \
  {                                                                                                                        \
    std::cerr << "Failed to set the global object(" << #name << ") for worker context, reason: " << e.what() << std::endl; \
  }
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
      workerContext->SetEmbedderData(ContextEmbedderIndex::kScriptingContextExternal, v8::External::New(isolate, this));
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

  bool DOMScriptingContext::compile(shared_ptr<DOMScript> script, const std::string &source, bool isTypeScript)
  {
    assert(isContextInitialized);
    auto context = v8ContextStore.Get(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope contextScope(context);

    if (!script->compile(isolate, source, isTypeScript))
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

  bool DOMScriptingContext::compileAsSyntheticModule(shared_ptr<DOMModule> module, SyntheticModuleType type, const void *sourceData, size_t sourceByteLength)
  {
    assert(isContextInitialized);
    auto context = v8ContextStore.Get(isolate);
    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope contextScope(context);

    if (!module->compileAsSyntheticModule(isolate, type, sourceData, sourceByteLength))
      return false;

    hashToModuleMap.insert({module->getModuleHash(), module});
    module->link(isolate);
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

  void DOMScriptingContext::tryImportModule(const string &url, const bool disableCache,
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
        std::cerr << "Failed to load the module: " << module->url << ", the extension is not supported." << std::endl;
      }
    };
    runtimeContext->fetchArrayBufferLikeResource(url, onModuleSourceLoaded);
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
        WindowProxyPropertyEnumeratorCallback,
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

  bool DOMClassicScript::compile(v8::Isolate *isolate, const string &sourceStr, bool isTypeScript)
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

    // Transpile the TypeScript source code if necessary.
    string scriptSourceString;
    if (isTypeScript)
    {
      try
      {
        scriptSourceString = crates::TypeScriptTranspiler::Transpile(sourceStr);
      }
      catch (const std::exception &e)
      {
        std::cerr << "Failed to compile TypeScript: " << e.what() << std::endl;
        return false;
      }
    }
    else
    {
      scriptSourceString = sourceStr;
    }

    // create the script
    auto sourceString = v8::String::NewFromUtf8(isolate, scriptSourceString.c_str()).ToLocalChecked();
    v8::ScriptCompiler::Source source(sourceString, origin);

    // compile the script
    auto context = isolate->GetCurrentContext();
    auto maybeScript = v8::ScriptCompiler::Compile(context, &source, v8::ScriptCompiler::kNoCompileOptions);

    v8::Local<v8::Script> script;
    if (maybeScript.ToLocal(&script))
    {
      scriptStore.Reset(isolate, script);
      return true;
    }
    else
    {
      std::cerr << "Failed to compile script" << std::endl;
      std::cerr << "  URL: " << url << std::endl;
      std::cerr << "  Source: " << std::endl;
      std::cerr << scriptSourceString << std::endl;
      throw std::runtime_error("Failed to compile script(" + url + ")");
    }
  }

  void DOMClassicScript::evaluate(v8::Isolate *isolate)
  {
    v8::HandleScope scope(isolate);

    auto script = scriptStore.Get(isolate);
    auto context = isolate->GetCurrentContext();

    v8::TryCatch tryCatch(isolate);
    v8::MaybeLocal<v8::Value> result = script->Run(context);
    if (result.IsEmpty())
    {
      cerr << "#" << endl;
      cerr << "# Failed to execute script" << endl;
      cerr << "# URL: " << url << endl;
      cerr << "#" << endl;

      if (tryCatch.HasCaught())
      {
        auto stackTrace = tryCatch.StackTrace(context).ToLocalChecked();
        if (!stackTrace.IsEmpty())
        {
          v8::String::Utf8Value stackTraceUtf8(isolate, stackTrace);
          std::string stackTraceStr(*stackTraceUtf8, stackTraceUtf8.length());
          std::cerr << "# " << stackTraceStr << std::endl;
        }
        else
        {
          v8::Local<v8::Message> message = tryCatch.Message();
          v8::String::Utf8Value messageUtf8(isolate, message->Get());
          std::string messageStr(*messageUtf8, messageUtf8.length());
          std::cerr << "# Error: " << messageStr << std::endl;
        }
      }
    }
  }

  v8::MaybeLocal<v8::Module> DOMModule::ResolveModuleCallback(v8::Local<v8::Context> context,
                                                              v8::Local<v8::String> specifier,
                                                              v8::Local<v8::FixedArray> importAssertions,
                                                              v8::Local<v8::Module> referrer)
  {
    v8::Isolate *isolate = context->GetIsolate();

    auto specifier_utf8 = v8::String::Utf8Value(isolate, specifier);
    string specifierStr(*specifier_utf8, specifier_utf8.length());

    DOMScriptingContext *scriptingContext = DOMScriptingContext::GetCurrent(context);
    if (scriptingContext == nullptr)
    {
      std::cerr << "request for '" << specifierStr << "' failed, scripting context is valid" << std::endl;
      return v8::MaybeLocal<v8::Module>();
    }

    auto dependent = scriptingContext->getModuleFromV8(referrer);
    if (dependent == nullptr)
    {
      std::cerr << "request for '" << specifierStr << "' is from invalid module" << std::endl;
      return v8::MaybeLocal<v8::Module>();
    }
    if (dependent->resolveCache.count(specifierStr) != 1)
    {
      std::cerr << "request for '" << specifierStr << "' is not in cache" << std::endl;
      return v8::MaybeLocal<v8::Module>();
    }

    auto module = dependent->resolveCache[specifierStr];
    auto res = module->moduleStore.Get(isolate);
    return res;
  }

  v8::MaybeLocal<v8::Value> DOMModule::SyntheticModuleEvaluationStepsCallback(v8::Local<v8::Context> context,
                                                                              v8::Local<v8::Module> v8module)
  {
    v8::Isolate *isolate = context->GetIsolate();
    v8::HandleScope handleScope(isolate);

    DOMScriptingContext *scriptingContext = DOMScriptingContext::GetCurrent(context);
    if (scriptingContext == nullptr)
      return v8::MaybeLocal<v8::Value>();

    shared_ptr<DOMModule> module = scriptingContext->getModuleFromV8(v8module);
    if (module == nullptr)
      return v8::MaybeLocal<v8::Value>();

    v8::Local<v8::Promise::Resolver> resolver;
    if (!v8::Promise::Resolver::New(context).ToLocal(&resolver))
      return v8::MaybeLocal<v8::Value>();

    auto defaultExportName = v8::String::NewFromUtf8(isolate, "default").ToLocalChecked();
    USE(v8module->SetSyntheticModuleExport(isolate, defaultExportName, module->getExports(isolate)));

    resolver->Resolve(context, v8::Undefined(isolate)).ToChecked();
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

  bool DOMModule::compile(v8::Isolate *isolate, const string &sourceStr, bool isTypeScript)
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

    // Transpile the TypeScript source code if necessary.
    string scriptSourceString;
    if (isTypeScript)
    {
      try
      {
        scriptSourceString = crates::TypeScriptTranspiler::Transpile(sourceStr);
      }
      catch (const std::exception &e)
      {
        std::cerr << "Failed to compile TypeScript: " << e.what() << std::endl;
        return false;
      }
    }
    else
    {
      scriptSourceString = sourceStr;
    }

    // create the script
    auto sourceString = v8::String::NewFromUtf8(isolate, scriptSourceString.c_str()).ToLocalChecked();
    v8::ScriptCompiler::Source source(sourceString, origin);
    v8::ScriptCompiler::CompileOptions options = v8::ScriptCompiler::kNoCompileOptions;

    // compile the script
    v8::Local<v8::Module> module;
    auto context = isolate->GetCurrentContext();

    if (!v8::ScriptCompiler::CompileModule(isolate, &source, options).ToLocal(&module))
    {
      std::cerr << "#" << std::endl;
      std::cerr << "# Occurred module compilation error" << std::endl;
      std::cerr << "# URL: " << url << std::endl;
      std::cerr << "#" << std::endl;
      std::cerr << sourceStr << std::endl;
      return false;
    }
    else
    {
      moduleStore.Reset(isolate, module);
      return true;
    }
  }

  bool DOMModule::compileAsSyntheticModule(v8::Isolate *isolate, SyntheticModuleType type, const void *sourceData, size_t sourceByteLength)
  {
    v8::HandleScope scope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    v8::TryCatch tryCatch(isolate);
    v8::Local<v8::String> urlString = v8::String::NewFromUtf8(isolate, url.c_str()).ToLocalChecked();
    v8::Local<v8::String> defaultExportName = v8::String::NewFromUtf8(isolate, "default").ToLocalChecked();

    switch (type)
    {
    case SyntheticModuleType::JSON:
    {
      v8::Local<v8::Value> sourceJsonObject;
      v8::Local<v8::String> sourceString = v8::String::NewFromUtf8(isolate, static_cast<const char *>(sourceData), v8::NewStringType::kNormal, sourceByteLength).ToLocalChecked();
      if (v8::JSON::Parse(context, sourceString).ToLocal(&sourceJsonObject))
        syntheticModuleNamespaceStore.Reset(isolate, sourceJsonObject);
      break;
    }
    case SyntheticModuleType::ArrayBuffer:
    {
      v8::Local<v8::ArrayBuffer> sourceArrayBuffer = v8::ArrayBuffer::New(isolate, sourceByteLength);
      memcpy(sourceArrayBuffer->GetBackingStore()->Data(), sourceData, sourceByteLength);
      syntheticModuleNamespaceStore.Reset(isolate, sourceArrayBuffer.As<v8::Value>());
      break;
    }
    default:
      break;
    }

    if (tryCatch.HasCaught())
    {
      std::cerr << "#" << std::endl;
      std::cerr << "# Occurred synthetic module compilation error" << std::endl;
      std::cerr << "# URL: " << url << std::endl;
      {
        // Print the exception message
        v8::Local<v8::Message> message = tryCatch.Message();
        v8::String::Utf8Value messageUtf8(isolate, message->Get());
        std::string messageStr(*messageUtf8, messageUtf8.length());
        std::cerr << "# Error: " << messageStr << std::endl;
      }
      std::cerr << "#" << std::endl;
      return false;
    }

    v8::Local<v8::Module> module = v8::Module::CreateSyntheticModule(isolate,
                                                                     urlString, {defaultExportName}, SyntheticModuleEvaluationStepsCallback);
    moduleStore.Reset(isolate, module);
    return true;
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

  v8::Local<v8::Value> DOMModule::getExports(v8::Isolate *isolate)
  {
    v8::EscapableHandleScope handleScope(isolate);
    v8::Local<v8::Module> module = moduleStore.Get(isolate);
    if (module.IsEmpty())
      return handleScope.Escape(v8::Undefined(isolate));

    if (module->IsSyntheticModule())
    {
      if (syntheticModuleNamespaceStore.IsEmpty())
        return handleScope.Escape(v8::Undefined(isolate));
      else
        return handleScope.Escape(syntheticModuleNamespaceStore.Get(isolate));
    }
    else
    {
      auto modNamespace = module->GetModuleNamespace();
      if (modNamespace.IsEmpty())
        return handleScope.Escape(v8::Undefined(isolate));
      else
        return handleScope.Escape(modNamespace);
    }
  }

  void DOMModule::link(v8::Isolate *isolate)
  {
    v8::HandleScope scope(isolate);
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

  bool DOMModule::instantiate(v8::Isolate *isolate)
  {
    v8::HandleScope scope(isolate);
    v8::Local<v8::Module> module = moduleStore.Get(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
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

    v8::Isolate *isolate = v8::Isolate::GetCurrent();
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

  void DOMModule::doEvaluate(v8::Isolate *isolate)
  {
    if (evaluatedOnce)
      return;
    evaluatedOnce = true;

    v8::HandleScope scope(isolate);
    auto module = moduleStore.Get(isolate);
    auto context = isolate->GetCurrentContext();

    if (module->IsSyntheticModule()) // Synthetic module should not be evaluated
      return;
    if (module->GetStatus() != v8::Module::Status::kInstantiated)
    {
      std::cerr << "Failed to evaluate the module: " << url << ", the module is not instantiated." << std::endl;
      return;
    }

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
        if (!resultValue->IsPromise())
        {
          std::cerr << "Failed to execute script: the result is not a promise" << std::endl;
        }
        else
        {
          auto promise = v8::Local<v8::Promise>::Cast(resultValue);
          auto resolveCallback = [](const v8::FunctionCallbackInfo<v8::Value> &info)
          {
            v8::Isolate *isolate = info.GetIsolate();
            v8::Local<v8::Context> context = isolate->GetCurrentContext();
            v8::Context::Scope contextScope(context);
            v8::HandleScope handleScope(isolate);
            assert(info.Length() > 0);
            info.GetReturnValue().Set(info[0]);
          };
          auto rejectCallback = [](const v8::FunctionCallbackInfo<v8::Value> &info)
          {
            v8::Isolate *isolate = info.GetIsolate();
            v8::Local<v8::Context> context = isolate->GetCurrentContext();
            v8::Context::Scope contextScope(context);
            v8::HandleScope handleScope(isolate);
            v8::String::Utf8Value message(info.GetIsolate(), info[0]);
            std::cerr << "Failed to execute script: " << *message << std::endl;
          };
          v8::Local<v8::Function> resolveFunc = v8::Function::New(context, resolveCallback).ToLocalChecked();
          v8::Local<v8::Function> rejectFunc = v8::Function::New(context, rejectCallback).ToLocalChecked();
        }
      }
    }
  }
}
