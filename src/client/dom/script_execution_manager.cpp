#include "./script_execution_manager.hpp"
#include <iostream>
#include <algorithm>

namespace dom
{
  using namespace std;

  ScriptExecutionManager::ScriptExecutionManager(shared_ptr<IScriptExecutionContext> context)
      : context_(context)
  {
  }

  void ScriptExecutionManager::handleScript(shared_ptr<IScript> script)
  {
    if (!script)
    {
      return;
    }
    switch (script->getType())
    {
    case ScriptType::Async:
      handleAsyncScript(script);
      break;
    case ScriptType::Defer:
      handleDeferScript(script);
      break;
    case ScriptType::Normal:
    default:
      handleNormalScript(script);
      break;
    }
  }

  void ScriptExecutionManager::handleNormalScript(shared_ptr<IScript> script)
  {
    // Normal script: block parsing until loaded and executed
    if (parsingPaused_)
    {
      // If parsing is already paused by another script, queue this script
      pendingScripts_.push(script);
      // removed debug log
    }
    else
    {
      currentBlockingScript_ = script;
      parsingPaused_ = true;

      auto context = context_.lock();
      if (context)
      {
        context->pauseParsing();
      }

      if (script->isInline())
      {
        // Inline scripts still require compilation before execution.
        if (script->isReady())
        {
          // removed debug log
          executeScript(script);
          currentBlockingScript_.reset();
          resumeParsing();
        }
        else
        {
          // Compile inline script first via load(), then execute.
          startScriptLoading(script, [this, script](bool success)
                             {
                               if (success)
                               {
                                 // removed debug log
                                 executeScript(script);
                               }
                               else
                               {
                                 // removed debug log
                               }
                               currentBlockingScript_.reset();
                               resumeParsing(); });
        }
      }
      else
      {
        // External script needs loading
        // removed debug log
        startScriptLoading(script, [this, script](bool success)
                           {
                             if (success)
                             {
                               executeScript(script);
                             }
                             else
                             {
                               // removed debug log
                             }
                             currentBlockingScript_.reset();
                             resumeParsing(); });
      }
    }
  }

  void ScriptExecutionManager::handleAsyncScript(shared_ptr<IScript> script)
  {
    // Async script: load in parallel, execute immediately when ready
    // removed debug log
    startScriptLoading(script, [this, script](bool success)
                       {
                         if (success)
                         {
                           // removed debug log
                           executeScript(script);
                         }
                         else
                         {
                           // removed debug log
                         } });
  }

  void ScriptExecutionManager::handleDeferScript(shared_ptr<IScript> script)
  {
    // Defer script: load in parallel, execute in order after parsing
    // removed debug log
    deferQueue_.push(script);

    // Start loading but don't execute yet
    startScriptLoading(script, [this, script](bool success)
                       {
                         if (success)
                         {
                           // removed debug log
                           // Don't execute here - wait for onParsingFinished
                           // But check if we can execute defer scripts now
                           if (parsingFinished_ && !parsingPaused_ && !currentBlockingScript_)
                           {
                             executeDeferredScripts();
                           }
                         }
                         else
                         {
                           // removed debug log
                         } });
  }

  void ScriptExecutionManager::onParsingFinished()
  {
    // removed debug log
    parsingFinished_ = true;

    // Only execute defer scripts if there are no blocking scripts and no pending scripts
    if (!currentBlockingScript_ && pendingScripts_.empty() && !parsingPaused_)
    {
      executeDeferredScripts();
    }
  }

  void ScriptExecutionManager::onScriptStateChanged(shared_ptr<IScript> script, ScriptState newState)
  {
    // removed debug log

    // This method can be used for additional state change handling if needed
    // Currently, the loading callbacks handle execution directly
  }

  void ScriptExecutionManager::resumeParsing()
  {
    parsingPaused_ = false;

    auto context = context_.lock();
    if (context)
    {
      context->resumeParsing();
    }

    // Process next pending script if any
    if (!pendingScripts_.empty())
    {
      processNextPendingScript();
    }
    else
    {
      // If parsing is finished and there are no more blocking scripts, execute defer scripts
      if (parsingFinished_ && !currentBlockingScript_)
      {
        executeDeferredScripts();
      }
    }
  }

  void ScriptExecutionManager::processNextPendingScript()
  {
    if (pendingScripts_.empty())
      return;

    auto nextScript = pendingScripts_.front();
    pendingScripts_.pop();

    // removed debug log

    if (nextScript->isInline())
    {
      // Inline scripts: ensure compilation before execution
      if (nextScript->isReady())
      {
        executeScript(nextScript);
        // Continue processing remaining scripts
        resumeParsing();
      }
      else
      {
        startScriptLoading(nextScript, [this, nextScript](bool success)
                           {
                             if (success)
                             {
                               executeScript(nextScript);
                             }
                             else
                             {
                               // removed debug log
                             }
                             // Continue processing remaining scripts
                             resumeParsing(); });
      }
    }
    else
    {
      // Start loading the next external script
      currentBlockingScript_ = nextScript;
      parsingPaused_ = true;

      auto context = context_.lock();
      if (context)
      {
        context->pauseParsing();
      }

      startScriptLoading(nextScript, [this, nextScript](bool success)
                         {
                           if (success)
                           {
                             executeScript(nextScript);
                           }
                           else
                           {
                             // removed debug log
                           }
                           currentBlockingScript_.reset();
                           resumeParsing(); });
    }
  }

  void ScriptExecutionManager::executeScript(shared_ptr<IScript> script)
  {
    if (!script)
    {
      // removed debug log
      return;
    }

    // removed debug log

    try
    {
      script->execute();
      // removed debug log
    }
    catch (const exception &e)
    {
      // removed debug log
    }
    catch (...)
    {
      // removed debug log
    }
  }

  void ScriptExecutionManager::executeDeferredScripts()
  {
    // removed debug log

    // Execute ready defer scripts in order
    while (!deferQueue_.empty())
    {
      auto script = deferQueue_.front();

      // Check if the first script in queue is ready
      if (!script->isReady())
      {
        // removed debug log
        // Stop here - we must execute defer scripts in order
        return;
      }

      // Script is ready, execute it
      deferQueue_.pop();
      executeScript(script);
    }

    // All defer scripts executed, check if we can fire DOMContentLoaded
    checkAndFireDOMContentLoaded();
  }

  void ScriptExecutionManager::checkAndFireDOMContentLoaded()
  {
    if (domContentLoadedFired_ || !parsingFinished_)
      return;

    // All deferred scripts should be executed by now
    if (deferQueue_.empty())
    {
      // removed debug log
      domContentLoadedFired_ = true;

      auto context = context_.lock();
      if (context)
      {
        context->dispatchDOMContentLoaded();
      }
    }
  }

  void ScriptExecutionManager::startScriptLoading(shared_ptr<IScript> script,
                                                  function<void(bool success)> onComplete)
  {
    if (!script)
      return;

    // Start loading the script
    script->load([script, onComplete](bool success)
                 {
                   if (onComplete)
                   {
                     onComplete(success);
                   } });
  }
}
