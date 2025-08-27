#include "./browsing_context.hpp"
#include "../html/html_script_element.hpp"

namespace dom
{
  std::size_t BrowsingContext::registerScriptForExecution(std::shared_ptr<HTMLScriptElement> script)
  {
    std::size_t scriptId = nextScriptId++;
    scriptExecutionQueue.emplace_back(scriptId, std::weak_ptr<HTMLScriptElement>(script));
    // Try to execute if this is the first script in the queue
    tryExecuteNextScript();
    return scriptId;
  }

  void BrowsingContext::tryExecuteNextScript()
  {
    while (!scriptExecutionQueue.empty())
    {
      auto &handle = scriptExecutionQueue.front();
      auto script = handle.scriptElement.lock();

      // If script was destroyed, remove from queue and continue
      if (!script)
      {
        scriptExecutionQueue.pop_front();
        continue;
      }

      // Check if the script is ready to execute (compiled and not yet executed)
      if (script->isReadyToExecute())
      {
        script->executeScriptFromQueue();
        return; // Script will call notifyScriptExecutionComplete when done
      }

      // Script not ready yet, stop trying
      break;
    }
  }

  void BrowsingContext::notifyScriptExecutionComplete(std::size_t scriptId)
  {
    if (!scriptExecutionQueue.empty() && scriptExecutionQueue.front().scriptId == scriptId)
    {
      scriptExecutionQueue.pop_front();
      // Try to execute the next script in the queue
      tryExecuteNextScript();
    }
  }
}