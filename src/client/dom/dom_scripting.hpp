#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <node/v8.h>
#include <node/node.h>

using namespace std;

namespace dom
{
  enum ContextEmbedderIndex : int
  {
    kMagicIndex = 255,
    kEnvironmentObject,
    kSandboxObject,
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

  class DocumentRenderingContext;
  class DOMScript;
  class DOMClassicScript;
  class DOMModule;

  /**
   * The context class for DOM scripting, this is the implementation of how the DOM compiles and evaluates the script.
   */
  class DOMScriptingContext : public enable_shared_from_this<DOMScriptingContext>
  {
  private:
    static void PropertyGetterCallback(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    static v8::MaybeLocal<v8::Promise> ImportModuleDynamicallyCallback(v8::Local<v8::Context> context,
                                                                       v8::Local<v8::Data> hostDefinedOptions,
                                                                       v8::Local<v8::Value> resourceName,
                                                                       v8::Local<v8::String> specifier,
                                                                       v8::Local<v8::FixedArray> importAssertions);

  public:
    DOMScriptingContext();

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
     * Set the script's `document` v8::Value instance.
     */
    void setDocumentValue(v8::Local<v8::Value> value);
    /**
     * Make the v8::Context for the script.
     */
    void makeV8Context();
    /**
     * Create a new DOM script object.
     *
     * @param context The document rendering context.
     * @param url The URL of the script.
     * @param type The type of the script: classic or module.
     * @returns The new DOM script object.
     */
    shared_ptr<DOMScript> create(shared_ptr<DocumentRenderingContext> context, const string &url, SourceTextType type);
    /**
     * Compile the given script.
     *
     * @param script The script to compile.
     * @param source The source code of the script.
     * @returns Whether the script is compiled successfully.
     */
    bool compile(shared_ptr<DOMScript> script, const std::string &source);
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

  private:
    v8::Isolate *isolate;
    v8::Global<v8::Value> documentValue;
    v8::Global<v8::Context> scriptingContext;
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
    DOMScript(SourceTextType sourceTextType, shared_ptr<DocumentRenderingContext> context);
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
    weak_ptr<DocumentRenderingContext> documentRenderingContext;
  };

  class DOMClassicScript : public DOMScript
  {
  public:
    DOMClassicScript(shared_ptr<DocumentRenderingContext> context);
    ~DOMClassicScript() override;

  public:
    bool compile(v8::Isolate *isolate, const string &sourceStr) override;
    void evaluate(v8::Isolate *isolate) override;

  private:
    v8::Global<v8::Script> scriptStore;
  };

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

  public:
    DOMModule(shared_ptr<DocumentRenderingContext> context);
    ~DOMModule() override;

  public:
    bool compile(v8::Isolate *isolate, const string &sourceStr) override;
    void evaluate(v8::Isolate *isolate) override;
    int getModuleHash();
    /**
     * Get the URL string by the specifier.
     *
     * @returns The module URL string to be fetched.
     */
    string getUrlBySpecifier(const string &specifier);
    /**
     * Set the callback when the module linking is finished.
     *
     * @param callback The callback to call when the module linking is finished.
     */
    void setLinkFinishedCallback(function<void(shared_ptr<DOMModule>)> callback);

  private:
    void link(v8::Isolate *isolate);
    bool instantiate(v8::Isolate *isolate);
    void handleModuleLoaded(const string &specifier, shared_ptr<DOMModule> module);
    void checkLinkFinished();
    void onLinkFinished();
    void doEvaluate(v8::Isolate *isolate);

  private:
    v8::Global<v8::Module> moduleStore;
    unordered_map<string, shared_ptr<DOMModule>> resolveCache;
    bool linked = false;
    size_t validModuleRequestsCount;
    function<void(shared_ptr<DOMModule>)> linkFinishedCallback;
    bool evaluatedOnce = false;
    bool evaluationScheduled = false;
  };
}
