#include "./browsing_context.hpp"
#include "../html/html_script_element.hpp"

namespace dom
{
  void BrowsingContext::registerScriptForExecution(std::shared_ptr<HTMLScriptElement> script)
  {
    scriptExecutionQueue.push_back(script);
    // Try to execute if this is the first script in the queue
    tryExecuteNextScript();
  }

  void BrowsingContext::tryExecuteNextScript()
  {
    if (scriptExecutionQueue.empty())
      return;

    auto nextScript = scriptExecutionQueue.front();
    // Check if the script is ready to execute (compiled and not yet executed)
    if (nextScript && nextScript->isReadyToExecute())
    {
      nextScript->executeScriptFromQueue();
    }
  }

  void BrowsingContext::notifyScriptExecutionComplete(std::shared_ptr<HTMLScriptElement> script)
  {
    if (!scriptExecutionQueue.empty() && scriptExecutionQueue.front() == script)
    {
      scriptExecutionQueue.pop_front();
      // Try to execute the next script in the queue
      tryExecuteNextScript();
    }
  }
}