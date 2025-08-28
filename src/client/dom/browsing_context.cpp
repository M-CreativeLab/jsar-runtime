#include <idgen.hpp>
#include "./browsing_context.hpp"
#include "../html/html_script_element.hpp"

namespace dom
{
  uint32_t BrowsingContext::registerScriptForExecution(std::shared_ptr<HTMLScriptElement> script)
  {
    static TrIdGenerator idgen(0x1);
    uint32_t script_id = idgen.get();
    script_execution_queue.emplace_back(script_id, std::weak_ptr<HTMLScriptElement>(script));

    // Try to execute if this is the first script in the queue
    tryExecuteNextScript();
    return script_id;
  }

  void BrowsingContext::tryExecuteNextScript()
  {
    while (!script_execution_queue.empty())
    {
      auto &handle = script_execution_queue.front();
      auto script = handle.scriptElement.lock();

      // If script was destroyed, remove from queue and continue
      if (!script)
      {
        script_execution_queue.pop_front();
        continue;
      }

      // Execute the script and it will return false if not ready yet
      if (!script->executeScriptFromQueue())
        return; // Script will call notifyScriptExecutionComplete when done

      // Script not ready yet, stop trying
      break;
    }
  }

  void BrowsingContext::notifyScriptExecutionComplete(uint32_t script_id)
  {
    if (!script_execution_queue.empty() &&
        script_execution_queue.front().scriptId == script_id)
    {
      script_execution_queue.pop_front();
      tryExecuteNextScript();
    }
  }
}
