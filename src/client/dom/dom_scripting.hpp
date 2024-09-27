#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <node/v8.h>
#include <node/node.h>
#include "common/utility.hpp"

using namespace std;

template <typename T>
inline void USE(T &&) {}

namespace dom
{
  enum ContextEmbedderIndex : int
  {
    kSandboxObject = 50,
    kScriptingContextExternal,
    kContextTag,
  };

  enum HostDefinedOptions : int
  {
    kType = 10,
    kID,
    kLength,
  };

  enum class SourceTextType
  {
    Classic,
    ESM,
  };

  /**
   * The types for the synthetic module.
   */
  enum class SyntheticModuleType
  {
    /**
     * A JSON module such as `import data from './data.json'`.
     */
    JSON,
    /**
     * An ArrayBuffer module such as `import data from './data.bin'`.
     */
    ArrayBuffer,
    /**
     * TODO: How to support the following types?
     * 
     * For example, when we wanna import an image as an instance of `HTMLImageElement`:
     * 
     * ```js
     * import image from './image.png';
     * console.info(image instanceof HTMLImageElement); // true
     * ```
     * 
     * The same for other types:
     * 
     * - import blob files as an instance of `Blob`
     * - import audio files as an instance of `Audio`
     * - import video files as an instance of `Video`
     * - import wasm files as an instance of `WebAssembly`
     */
    Blob,
    Audio,
    Video,
    Image,
    WebAssembly,
  };

  /**
   * Copied from `src/node_context_data.h` in Node.js source code because this is useful for context validation.
   */
  class ContextEmbedderTag
  {
  public:
    /**
     * Tag the given context as a JSAR context
     */
    static inline void TagMyContext(v8::Local<v8::Context> context)
    {
      // Used by ContextEmbedderTag::IsMyContext to know that we are on a node
      // context.
      context->SetAlignedPointerInEmbedderData(
          ContextEmbedderIndex::kContextTag,
          ContextEmbedderTag::kMyContextTagPtr);
    }

    static inline bool IsMyContext(v8::Local<v8::Context> context)
    {
      if (TR_UNLIKELY(context.IsEmpty()))
      {
        return false;
      }
      if (TR_UNLIKELY(context->GetNumberOfEmbedderDataFields() <=
                      ContextEmbedderIndex::kContextTag))
      {
        return false;
      }
      if (TR_UNLIKELY(context->GetAlignedPointerFromEmbedderData(
                          ContextEmbedderIndex::kContextTag) !=
                      ContextEmbedderTag::kMyContextTagPtr))
      {
        return false;
      }
      return true;
    }

  private:
    static void *const kMyContextTagPtr;
    static int const kMyContextTag;

    ContextEmbedderTag() = delete;
  };

  class RuntimeContext;
  class DOMScript;
  class DOMClassicScript;
  class DOMModule;

  /**
   * The context class for DOM scripting, this is the implementation of how the DOM compiles and evaluates the script.
   */
  class DOMScriptingContext : public std::enable_shared_from_this<DOMScriptingContext>
  {
  public:
    static DOMScriptingContext *GetCurrent(v8::Local<v8::Context> context);

  private:
    static void PropertyGetterCallback(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void WindowProxyPropertyGetterCallback(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static void WorkerSelfProxyPropertyGetterCallback(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static v8::MaybeLocal<v8::Promise> ImportModuleDynamicallyCallback(v8::Local<v8::Context> context,
                                                                       v8::Local<v8::Data> hostDefinedOptions,
                                                                       v8::Local<v8::Value> resourceName,
                                                                       v8::Local<v8::String> specifier,
                                                                       v8::Local<v8::FixedArray> importAssertions);

  public:
    DOMScriptingContext(shared_ptr<RuntimeContext> runtimeContext);

  public:
    /**
     * Enable the dynamic import for the script.
     *
     * Internally it will call `v8::Isolate`'s `SetHostImportModuleDynamicallyCallback` to set the dynamic import callback, it causes the
     * `import()` calls in different contexts will be handled by the same callback.
     *
     * These scripts are running inside the Node.js, that has its own callback for dynamic `import()`, and JSAR esm implementation can't work
     * well with it, so we need to make sure the Node.js script won't use `import()` anymore.
     */
    void enableDynamicImport();

    /**
     * Make a v8::Context for the main script.
     *
     * @param documentValue The document object to be used in the main script.
     */
    void makeMainContext(v8::Local<v8::Value> documentValue);

    /**
     * Make a v8::Context for the worker script.
     */
    void makeWorkerContext();

    /**
     * Create a new DOM script object.
     *
     * @param context The runtime context.
     * @param url The URL of the script.
     * @param type The type of the script: classic or module.
     * @returns The new DOM script object.
     */
    shared_ptr<DOMScript> create(shared_ptr<RuntimeContext> runtimeContext, const string &url, SourceTextType type);

    /**
     * Compile the given script.
     *
     * @param script The script to compile.
     * @param source The source code of the script.
     * @returns Whether the script is compiled successfully.
     */
    bool compile(shared_ptr<DOMScript> script, const std::string &source);

    /**
     * Compile the given script as a synthetic module.
     *
     * @param scriptModule The script module object to compile.
     * @param sourceData The source data of the script.
     * @param sourceLength The length of the source data.
     */
    bool compileAsSyntheticModule(shared_ptr<DOMModule> scriptModule, SyntheticModuleType type, const void *sourceData, size_t sourceLength);

    /**
     * Evaluate the given script.
     *
     * @param script The script to evaluate.
     */
    void evaluate(shared_ptr<DOMScript> script);

    /**
     * Get the `DOMModule` object from the given v8 module.
     *
     * @param v8module The v8 module object.
     * @returns The module object if found, otherwise nullptr.
     */
    shared_ptr<DOMModule> getModuleFromV8(v8::Local<v8::Module> v8module);

    /**
     * Get the `DOMModule` object from the given URL, it's used to check whether the URL is already loaded.
     *
     * @param url The URL of the module.
     * @returns The module object if found, otherwise nullptr.
     */
    shared_ptr<DOMModule> getModuleFromUrl(const string &url);

    /**
     * Update the import map from the given JSON string.
     *
     * @param json The JSON string of the import map.
     * @returns Whether the input json is valid.
     */
    bool updateImportMapFromJSON(const string &json);

    /**
     * Do the exact match import map for the given specifier, such as "three" -> "https://cdn.skypack.dev/three".
     *
     * @param specifier The specifier to match.
     * @returns The matched URL string if found, otherwise nullopt.
     * @see https://developer.mozilla.org/en-US/docs/Web/HTML/Element/script/type/importmap#bare_modules
     */
    optional<string> exactMatchImportMap(const string &specifier);

    /**
     * Do the prefix match import map for the given specifier, such as "three/foo" -> "https://cdn.skypack.dev/three/foo".
     *
     * @param specifier The specifier to match.
     * @returns The matched prefix URL string if found, otherwise nullopt.
     * @see https://developer.mozilla.org/en-US/docs/Web/HTML/Element/script/type/importmap#mapping_path_prefixes
     */
    optional<string> prefixMatchImportMap(const string &specifier);

    /**
     * Try to import a module from the given URL.
     *
     * @param url The URL of the module.
     * @param disableCache Whether to disable the cache.
     * @param loadedCallback The callback to call when the module is loaded.
     */
    void tryImportModule(const std::string &url, const bool disableCache, std::function<void(shared_ptr<DOMModule>)> loadedCallback);

    /**
     * Dispatch the global event in this scripting context.
     *
     * @param event The event object to dispatch.
     */
    bool dispatchEvent(v8::Local<v8::Object> event);

  private:
    /**
     * Create the `Window` or `WindowProxy` object for the main script.
     *
     * @param context The v8 context to create this object.
     * @returns The `Window` or `WindowProxy` object.
     */
    v8::Local<v8::Value> createWindowProxy(v8::Local<v8::Context> context);

    /**
     * Create the `WorkerGlobalScope.self` proxy object.
     *
     * @param context The v8 context to create this object.
     * @returns The `WorkerGlobalScope.self` proxy object.
     */
    v8::Local<v8::Value> createWorkerSelfProxy(v8::Local<v8::Context> context);

  private:
    v8::Isolate *isolate;
    v8::Global<v8::Context> v8ContextStore;
    shared_ptr<RuntimeContext> runtimeContext;
    unordered_map<int, shared_ptr<DOMModule>> hashToModuleMap;
    unordered_map<uint32_t, shared_ptr<DOMClassicScript>> idToScriptMap;
    unordered_map<uint32_t, shared_ptr<DOMModule>> idToModuleMap;
    unordered_map<string, shared_ptr<DOMModule>> urlToModuleMap;
    map<string, string> importExactMap;
    map<string, string> importPrefixMap;
    bool isContextInitialized = false;
  };

  /**
   * The virtual class for DOM script including the classic script and ECMAScript module.
   */
  class DOMScript : public enable_shared_from_this<DOMScript>
  {
  protected:
    DOMScript(SourceTextType sourceTextType, shared_ptr<RuntimeContext> runtimeContext);
    virtual ~DOMScript() = default;

  public:
    /**
     * Compile the script source code.
     *
     * @param isolate The V8 isolate.
     * @param source The source code of the script.
     * @param url The URL of the script.
     */
    virtual bool compile(v8::Isolate *isolate, const string &source) = 0;
    /**
     * Evaluate the script.
     *
     * @param isolate The V8 isolate.
     */
    virtual void evaluate(v8::Isolate *isolate) = 0;

  public:
    SourceTextType sourceTextType;
    uint32_t id;
    string url;
    bool crossOrigin = false;

  protected:
    weak_ptr<RuntimeContext> runtimeContext;
  };

  class DOMClassicScript : public DOMScript
  {
  public:
    DOMClassicScript(shared_ptr<RuntimeContext> runtimeContext);
    ~DOMClassicScript() override;

  public:
    bool compile(v8::Isolate *isolate, const string &sourceStr) override;
    void evaluate(v8::Isolate *isolate) override;

  private:
    v8::Global<v8::Script> scriptStore;
  };

  using ModuleLinkedCallback = std::function<void(std::shared_ptr<DOMModule>)>;

  class DOMModule : public DOMScript
  {
    friend class DOMScriptingContext;

    struct ModuleRequestInfo
    {
      string specifier;
      string url;
    };

  private:
    static v8::MaybeLocal<v8::Module> ResolveModuleCallback(v8::Local<v8::Context> context,
                                                            v8::Local<v8::String> specifier,
                                                            v8::Local<v8::FixedArray> importAssertions,
                                                            v8::Local<v8::Module> referrer);
    static v8::MaybeLocal<v8::Value> SyntheticModuleEvaluationStepsCallback(v8::Local<v8::Context> context,
                                                                            v8::Local<v8::Module> module);

  public:
    DOMModule(shared_ptr<RuntimeContext> runtimeContext);
    ~DOMModule() override;

  public:
    bool compile(v8::Isolate *isolate, const string &sourceStr) override;
    bool compileAsSyntheticModule(v8::Isolate *isolate, SyntheticModuleType type, const void *sourceData, size_t sourceByteLength);
    void evaluate(v8::Isolate *isolate) override;
    int getModuleHash();
    /**
     * Get the URL string by the specifier.
     *
     * @returns The module URL string to be fetched.
     */
    string getUrlBySpecifier(const string &specifier);
    /**
     * Register a callback to be called once when the module is linked or directly call the callback if the module is already linked.
     *
     * @param callback The callback to call when the module linking is finished.
     * @param checkLinked Whether to check the module is already linked and call the callback directly.
     */
    void registerLinkedCallback(ModuleLinkedCallback callback, bool checkLinked = true);

  private:
    v8::Local<v8::Value> getExports(v8::Isolate *isolate);
    void link(v8::Isolate *isolate);
    bool instantiate(v8::Isolate *isolate);
    void handleModuleLoaded(const string &specifier, shared_ptr<DOMModule> module);
    void checkLinkFinished();
    void onLinkFinished();
    void doEvaluate(v8::Isolate *isolate);

  private:
    v8::Global<v8::Module> moduleStore;
    v8::Global<v8::Value> syntheticModuleNamespaceStore;
    unordered_map<string, shared_ptr<DOMModule>> resolveCache;
    bool linked = false;
    size_t validModuleRequestsCount;
    vector<ModuleLinkedCallback> linkedCallbacks;
    bool evaluatedOnce = false;
    bool evaluationScheduled = false;
  };
}
