#pragma once

#include "./script_interfaces.hpp"
#include <queue>
#include <vector>
#include <memory>
#include <functional>

namespace dom
{
  /**
   * ScriptExecutionManager manages the loading and execution order of scripts
   * according to HTML specification, but decoupled from specific script element types.
   * 
   * This replaces the tightly-coupled ScriptLoader with a more flexible design
   * that works with any script source through the IScript interface.
   */
  class ScriptExecutionManager
  {
  public:
    /**
     * Constructor
     * @param context The execution context (usually a Document wrapper)
     */
    explicit ScriptExecutionManager(std::shared_ptr<IScriptExecutionContext> context);
    ~ScriptExecutionManager() = default;

    /**
     * Handle a script encountered during parsing
     * @param script The script to handle (via abstract interface)
     */
    void handleScript(std::shared_ptr<IScript> script);

    /**
     * Called when document parsing is finished
     * This will execute all deferred scripts in order
     */
    void onParsingFinished();

    /**
     * Called when a script's state changes (e.g., from Loading to Loaded)
     * @param script The script whose state changed
     * @param newState The new state
     */
    void onScriptStateChanged(std::shared_ptr<IScript> script, ScriptState newState);

    /**
     * Check if parsing should be paused (due to normal script loading)
     * @returns true if parsing should be paused
     */
    bool shouldPauseParsing() const
    {
      return parsingPaused_;
    }

    /**
     * Resume parsing after a blocking script has finished
     */
    void resumeParsing();

  private:
    /**
     * Handle different script types with appropriate strategies
     */
    void handleNormalScript(std::shared_ptr<IScript> script);
    void handleAsyncScript(std::shared_ptr<IScript> script);
    void handleDeferScript(std::shared_ptr<IScript> script);

    /**
     * Execute a script immediately
     * @param script The script to execute
     */
    void executeScript(std::shared_ptr<IScript> script);

    /**
     * Execute all deferred scripts in order
     */
    void executeDeferredScripts();

    /**
     * Process the next pending normal script
     */
    void processNextPendingScript();

    /**
     * Check and fire DOMContentLoaded event if conditions are met
     */
    void checkAndFireDOMContentLoaded();

    /**
     * Start loading a script with appropriate callbacks
     * @param script The script to load
     * @param onComplete Callback when loading completes
     */
    void startScriptLoading(std::shared_ptr<IScript> script,
                            std::function<void(bool success)> onComplete = nullptr);

  private:
    // Execution context
    std::weak_ptr<IScriptExecutionContext> context_;

    // Script queues (using abstract interface)
    std::queue<std::shared_ptr<IScript>> deferQueue_;
    std::queue<std::shared_ptr<IScript>> pendingScripts_;
    std::shared_ptr<IScript> currentBlockingScript_;

    // State management
    bool parsingPaused_ = false;
    bool parsingFinished_ = false;
    bool domContentLoadedFired_ = false;
  };
}
