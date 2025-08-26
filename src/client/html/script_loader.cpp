#include "./script_loader.hpp"
#include "./html_script_element.hpp"
#include "../dom/document.hpp"
#include "../dom/browsing_context.hpp"
#include <iostream>

namespace dom
{
  using namespace std;

  ScriptLoader::ScriptLoader(shared_ptr<Document> document)
      : document_(document)
  {
  }

  void ScriptLoader::handleScriptElement(shared_ptr<HTMLScriptElement> script)
  {
    if (script->async)
    {
      // Async script: load in parallel, execute when ready with proper scheduling
      startScriptLoading(script, [this, script]()
                         { scheduleAsyncScriptExecution(script); });
    }
    else if (script->defer)
    {
      // Defer script: load in parallel, execute in order after parsing
      deferQueue_.push(script);
      // Set up callback to mark script as ready when loaded
      startScriptLoading(script, [script]()
                         {
                           // Script is now loaded and compiled, but defer scripts should only execute
                           // after all synchronous scripts have finished and parsing is complete
                           // We don't execute here - let onParsingFinished handle defer script execution
                         });
    }
    else
    {
      // Normal script: block parsing until loaded and executed
      if (parsingPaused)
      {
        // If parsing is already paused by another script, queue this script
        pendingScripts_.push(script);
      }
      else
      {
        currentBlockingScript_ = script;
        parsingPaused = true;

        // Check if it's an inline script
        if (script->src.empty())
        {
          // Inline scripts don't need loading, execute immediately
          script->loadSource(); // This compiles the script
          executeScript(script);
          currentBlockingScript_.reset();
          resumeParsing();
        }
        else
        {
          // External script needs loading
          startScriptLoading(script, [this, script]()
                             {
                              executeScript(script);
                              currentBlockingScript_.reset();
                              resumeParsing(); });
        }
      }
    }
  }

  void ScriptLoader::onParsingFinished()
  {
    parsingFinished = true;

    // Only execute defer scripts if there are no blocking scripts and no pending scripts
    // Async scripts are executed immediately when loaded, so we don't need to handle them here
    if (currentBlockingScript_ == nullptr && pendingScripts_.empty() && !parsingPaused)
    {
      executeDeferredScripts();
    }
  }

  void ScriptLoader::onScriptLoaded(shared_ptr<HTMLScriptElement> script)
  {
    // This method can be used for additional script loaded handling if needed
    // Currently, the loading callbacks handle execution directly
  }

  void ScriptLoader::resumeParsing()
  {
    parsingPaused = false;

    // Process next pending script if any
    if (!pendingScripts_.empty())
    {
      auto nextScript = pendingScripts_.front();
      pendingScripts_.pop();

      // Check if it's an inline script
      if (nextScript->src.empty())
      {
        // Inline scripts need to be loaded (which compiles them) before execution
        nextScript->loadSource();
        executeScript(nextScript);
        // Continue processing remaining scripts
        resumeParsing();
      }
      else
      {
        // Start loading the next external script
        currentBlockingScript_ = nextScript;
        parsingPaused = true;
        startScriptLoading(nextScript, [this, nextScript]()
                           {
                            executeScript(nextScript);
                            currentBlockingScript_.reset();
                            resumeParsing(); });
      }
    }
    else
    {
      // If parsing is finished and there are no more blocking scripts, execute defer scripts
      // Note: async scripts are executed immediately when loaded, no need to handle them here
      if (parsingFinished && currentBlockingScript_ == nullptr)
      {
        executeDeferredScripts();
      }
    }

    // Notify document that parsing can continue
    // This would need to be integrated with the HTML parser
  }

  void ScriptLoader::executeScript(shared_ptr<HTMLScriptElement> script)
  {
    if (script == nullptr)
    {
      cout << "[DEBUG] ScriptLoader::executeScript called with null script" << endl;
      return;
    }

    cout << "[DEBUG] ScriptLoader executing script: src=" << (script->src.empty() ? "inline" : script->src) << endl;

    try
    {
      // Execute the script directly
      script->executeScript();
    }
    catch (const exception &e)
    {
      cerr << "Error executing script: " << e.what() << endl;
      // Continue execution even if script fails
    }
  }

  void ScriptLoader::executeDeferredScripts()
  {
    // Execute ready defer scripts in order
    while (!deferQueue_.empty())
    {
      auto script = deferQueue_.front();

      // Check if the first script in queue is ready
      if (!script->scriptCompiled)
      {
        // Stop here - we must execute defer scripts in order
        // The script's loading callback will call executeDeferredScripts again when ready
        return;
      }

      // Script is ready, execute it
      deferQueue_.pop();
      executeScript(script);
    }

    // All defer scripts executed, check if we can fire DOMContentLoaded
    checkAndFireDOMContentLoaded();
  }

  // executeReadyAsyncScripts method removed - async scripts now execute immediately

  void ScriptLoader::checkAndFireDOMContentLoaded()
  {
    if (domContentLoadedFired || !parsingFinished)
      return;

    // All deferred scripts should be executed by now
    if (deferQueue_.empty())
    {
      domContentLoadedFired = true;

      auto doc = document_.lock();
      if (doc)
      {
        // Fire DOMContentLoaded event
        doc->dispatchEvent(DOMEventType::DOMContentLoaded);
      }
    }
  }

  void ScriptLoader::startScriptLoading(shared_ptr<HTMLScriptElement> script,
                                        function<void()> callback)
  {
    if (script == nullptr)
      return;

    // Set the loading callback for the script
    if (callback)
    {
      script->setLoadingCallback(callback);
    }

    // Start loading the script
    script->loadSource();
  }

  void ScriptLoader::scheduleAsyncScriptExecution(shared_ptr<HTMLScriptElement> script)
  {
    // According to HTML spec, async scripts should execute as soon as they are loaded,
    // regardless of parsing state or other scripts. They should not wait for anything.
    // Execute immediately - this is the correct behavior for async scripts.
    executeScript(script);
  }
}
