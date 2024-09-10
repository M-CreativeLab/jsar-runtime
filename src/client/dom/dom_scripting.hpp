#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <node/v8.h>

using namespace std;

namespace dom
{
  enum ContextEmbedderIndex
  {
    kSandboxObject = 0x100,
  };

  enum class SourceTextType
  {
    Classic,
    ESM,
  };

  class DOMScriptingContext;

  /**
   * The virtual class for DOM script including the classic script and ECMAScript module.
   */
  class DOMScript
  {
  protected:
    DOMScript();
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
    uint32_t id;
    string url;
    bool crossOrigin = false;
  };

  /**
   * The context class for DOM scripting, this is the implementation of how the DOM compiles and evaluates the script.
   */
  class DOMScriptingContext : public enable_shared_from_this<DOMScriptingContext>
  {
  private:
    static void PropertyGetterCallback(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value> &info);

  public:
    DOMScriptingContext();

  public:
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
     * @param url The URL of the script.
     * @param type The type of the script: classic or module.
     * @returns The new DOM script object.
     */
    shared_ptr<DOMScript> create(const string &url, SourceTextType type);
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

  private:
    v8::Isolate *isolate;
    v8::Global<v8::Value> documentValue;
    v8::Global<v8::Context> scriptingContext;
    map<uint32_t, shared_ptr<DOMScript>> scripts;
    bool isContextInitialized = false;
  };

  class DOMClassicScript : public DOMScript
  {
  public:
    DOMClassicScript(shared_ptr<DOMScriptingContext> context);
    ~DOMClassicScript() override;

  public:
    bool compile(v8::Isolate *isolate, const string &sourceStr) override;
    void evaluate(v8::Isolate *isolate) override;

  private:
    v8::Global<v8::Script> scriptStore;
  };

  class DOMModule : public DOMScript
  {
  private:
    static v8::MaybeLocal<v8::Module> ResolveModuleCallback(v8::Local<v8::Context> context,
                                                            v8::Local<v8::String> specifier,
                                                            v8::Local<v8::FixedArray> importAssertions,
                                                            v8::Local<v8::Module> referrer);

  public:
    DOMModule(shared_ptr<DOMScriptingContext> context);
    ~DOMModule() override;

  public:
    bool compile(v8::Isolate *isolate, const string &sourceStr) override;
    void evaluate(v8::Isolate *isolate) override;

  private:
    bool instantiate(v8::Isolate *isolate);

  private:
    v8::Global<v8::Module> moduleStore;
    unordered_map<string, v8::Global<v8::Object>> resolveCache;
  };
}
