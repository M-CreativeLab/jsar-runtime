#pragma once

#include <queue>
#include <vector>
#include <memory>
#include <functional>

namespace dom
{
  class HTMLScriptElement;
  class Document;

  /**
   * ScriptLoader manages the loading and execution order of scripts according to HTML specification.
   * It handles:
   * - Normal scripts: Block parsing until loaded and executed
   * - Async scripts: Load in parallel, execute immediately when ready
   * - Defer scripts: Load in parallel, execute in order after parsing completes
   */
  class ScriptLoader
  {
  public:
    ScriptLoader(std::shared_ptr<Document> document);
    ~ScriptLoader() = default;

    /**
     * Handle a script element encountered during parsing.
     * 
     * @param script The script element to handle
     */
    void handleScriptElement(std::shared_ptr<HTMLScriptElement> script);

    /**
     * Called when document parsing is finished.
     * This will execute all deferred scripts in order.
     */
    void onParsingFinished();

    /**
     * Called when a script has finished loading.
     * 
     * @param script The script that finished loading
     */
    void onScriptLoaded(std::shared_ptr<HTMLScriptElement> script);

    /**
     * Check if parsing should be paused (due to normal script loading).
     * 
     * @returns true if parsing should be paused
     */
    bool shouldPauseParsing() const
    {
      return parsingPaused;
    }

    /**
     * Resume parsing after a blocking script has finished.
     */
    void resumeParsing();

  private:
    /**
     * Execute a script immediately.
     * 
     * @param script The script to execute
     */
    void executeScript(std::shared_ptr<HTMLScriptElement> script);

    /**
     * Execute all deferred scripts in order.
     */
    void executeDeferredScripts();

    /**
     * Check and fire DOMContentLoaded event if conditions are met.
     */
    void checkAndFireDOMContentLoaded();

    /**
     * Start loading a script.
     * 
     * @param script The script to load
     * @param callback Optional callback when loading completes
     */
    void startScriptLoading(std::shared_ptr<HTMLScriptElement> script,
                            std::function<void()> callback = nullptr);

    /**
     * Schedule async script execution to avoid blocking critical DOM operations.
     * 
     * @param script The async script to schedule
     */
    void scheduleAsyncScriptExecution(std::shared_ptr<HTMLScriptElement> script);


  private:
    std::weak_ptr<Document> document_;
    std::queue<std::shared_ptr<HTMLScriptElement>> deferQueue_;
    std::queue<std::shared_ptr<HTMLScriptElement>> pendingScripts_;
    std::shared_ptr<HTMLScriptElement> currentBlockingScript_;
    bool parsingPaused = false;
    bool parsingFinished = false;
    bool domContentLoadedFired = false;
  };
}
