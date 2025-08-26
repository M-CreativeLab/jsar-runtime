#pragma once

#include <string>
#include <memory>
#include <functional>

namespace dom
{
  /**
   * Script type enumeration defining execution behavior
   */
  enum class ScriptType
  {
    Normal, // Blocking script - pauses parsing until loaded and executed
    Async,  // Async script - loads in parallel, executes immediately when ready
    Defer   // Defer script - loads in parallel, executes in order after parsing
  };

  /**
   * Script state enumeration for tracking execution lifecycle
   */
  enum class ScriptState
  {
    Pending,   // Waiting to start loading
    Loading,   // Currently loading
    Loaded,    // Loaded and compiled, ready for execution
    Executing, // Currently executing
    Executed,  // Successfully executed
    Failed     // Execution failed
  };

  /**
   * Abstract script interface that decouples script execution logic
   * from specific script element implementations (HTMLScriptElement, etc.)
   */
  class IScript
  {
  public:
    virtual ~IScript() = default;

    /**
     * Get the script type (normal, async, defer)
     */
    virtual ScriptType getType() const = 0;

    /**
     * Get the current script state
     */
    virtual ScriptState getState() const = 0;

    /**
     * Get the script source identifier (URL for external scripts, "inline" for inline scripts)
     */
    virtual std::string getSource() const = 0;

    /**
     * Check if this is an inline script (no external URL)
     */
    virtual bool isInline() const = 0;

    /**
     * Start loading the script
     * @param callback Called when loading completes (success/failure)
     */
    virtual void load(std::function<void(bool success)> callback) = 0;

    /**
     * Execute the script
     * Should only be called when state is Loaded
     */
    virtual void execute() = 0;

    /**
     * Get a unique identifier for this script (for debugging and logging)
     */
    virtual std::string getId() const = 0;

    /**
     * Check if the script is ready for execution
     * (loaded and compiled successfully)
     */
    virtual bool isReady() const
    {
      return getState() == ScriptState::Loaded;
    }
  };

  /**
   * Abstract script execution context interface
   * Provides document-level operations needed by the script execution manager
   */
  class IScriptExecutionContext
  {
  public:
    virtual ~IScriptExecutionContext() = default;

    /**
     * Dispatch DOMContentLoaded event
     */
    virtual void dispatchDOMContentLoaded() = 0;

    /**
     * Check if document parsing has finished
     */
    virtual bool isParsingFinished() const = 0;

    /**
     * Notify that parsing should be paused
     */
    virtual void pauseParsing() = 0;

    /**
     * Notify that parsing can be resumed
     */
    virtual void resumeParsing() = 0;
  };
}