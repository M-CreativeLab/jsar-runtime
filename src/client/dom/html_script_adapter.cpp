#include "./html_script_adapter.hpp"
#include "./document.hpp"
#include <iostream>
#include <sstream>

namespace dom
{
  using namespace std;

  // HTMLScriptAdapter implementation

  HTMLScriptAdapter::HTMLScriptAdapter(shared_ptr<HTMLScriptElement> element)
      : element_(element)
  {
    if (!element_)
    {
      throw invalid_argument("HTMLScriptAdapter: element cannot be null");
    }
  }

  ScriptType HTMLScriptAdapter::getType() const
  {
    if (!element_)
      return ScriptType::Normal;

    // Module scripts semantics
    // - Parser-inserted module without async behaves like defer by default
    // - Non parser-inserted module behaves like async by default
    if (element_->isModuleScript())
    {
      if (!element_->parserInserted)
      {
        return ScriptType::Async; // dynamic module script
      }
      if (element_->async)
      {
        return ScriptType::Async;
      }
      return ScriptType::Defer; // default for parser-inserted module
    }

    // Classic scripts semantics
    // P0 rules per HTML spec for classic scripts:
    // - For non parser-inserted scripts, the default is async=true
    // - defer has no effect for non parser-inserted scripts
    // - For parser-inserted scripts, use attributes async/defer normally
    if (!element_->parserInserted)
    {
      // Non parser-inserted classic script: treat as async by default
      return ScriptType::Async;
    }

    if (element_->async)
      return ScriptType::Async;
    if (element_->defer)
      return ScriptType::Defer;
    return ScriptType::Normal;
  }

  ScriptState HTMLScriptAdapter::getState() const
  {
    if (!element_)
      return ScriptState::Failed;

    // Map HTMLScriptElement state to ScriptState
    if (element_->isScriptExecutedOnce())
    {
      return ScriptState::Executed;
    }
    else if (element_->scriptCompiled)
    {
      return ScriptState::Loaded;
    }
    else
    {
      // We don't have a direct "loading" state in HTMLScriptElement,
      // so we assume it's pending if not compiled yet
      return ScriptState::Pending;
    }
  }

  string HTMLScriptAdapter::getSource() const
  {
    if (!element_)
      return "null";

    return element_->src.empty() ? "inline" : element_->src;
  }

  bool HTMLScriptAdapter::isInline() const
  {
    if (!element_)
      return false;

    return element_->src.empty();
  }

  void HTMLScriptAdapter::load(function<void(bool success)> callback)
  {
    if (!element_)
    {
      if (callback)
        callback(false);
      return;
    }

    // Set up the loading callback
    if (callback)
    {
      element_->setLoadingCallback([callback](bool success)
                                   { callback(success); });
    }

    try
    {
      // Start loading the script
      element_->loadSource();
    }
    catch (const exception &e)
    {
      if (callback)
        callback(false);
    }
    catch (...)
    {
      if (callback)
        callback(false);
    }
  }

  void HTMLScriptAdapter::execute()
  {
    if (!element_)
    {
      return;
    }

    try
    {
      element_->executeScript();
    }
    catch (const exception &e)
    {
      throw; // Re-throw to let the execution manager handle it
    }
    catch (...)
    {
      throw; // Re-throw to let the execution manager handle it
    }
  }

  string HTMLScriptAdapter::getId() const
  {
    if (!element_)
      return "null-element";

    // Cache the ID to avoid regenerating it frequently
    if (cachedId_.empty())
    {
      ostringstream oss;
      if (element_->src.empty())
      {
        // For inline scripts, use a combination of element pointer and content hash
        oss << "inline-script-" << reinterpret_cast<uintptr_t>(element_.get());
      }
      else
      {
        oss << "external-script-" << element_->src;
      }
      cachedId_ = oss.str();
    }

    return cachedId_;
  }

  bool HTMLScriptAdapter::isReady() const
  {
    return getState() == ScriptState::Loaded;
  }

  // DocumentScriptContext implementation

  DocumentScriptContext::DocumentScriptContext(shared_ptr<Document> document)
      : document_(document)
  {
    if (!document)
    {
      throw invalid_argument("DocumentScriptContext: document cannot be null");
    }
  }

  void DocumentScriptContext::dispatchDOMContentLoaded()
  {
    auto doc = document_.lock();
    if (!doc)
    {
      return;
    }

    try
    {
      doc->dispatchEvent(DOMEventType::DOMContentLoaded);
    }
    catch (const exception &e)
    {
    }
    catch (...)
    {
    }
  }

  bool DocumentScriptContext::isParsingFinished() const
  {
    auto doc = document_.lock();
    if (!doc)
    {
      // If document is gone, assume parsing is finished
      return true;
    }

    // Check if document has a method to determine parsing state
    // For now, we'll assume parsing is finished if the document exists
    // This might need to be enhanced based on the actual Document implementation
    return true; // TODO: Implement proper parsing state check
  }

  void DocumentScriptContext::pauseParsing()
  {
    auto doc = document_.lock();
    if (!doc)
    {
      return;
    }

    // TODO: Implement actual parsing pause mechanism
    // This would need to integrate with the HTML parser
    // For now, this is a placeholder
  }

  void DocumentScriptContext::resumeParsing()
  {
    auto doc = document_.lock();
    if (!doc)
    {
      return;
    }

    // TODO: Implement actual parsing resume mechanism
    // This would need to integrate with the HTML parser
    // For now, this is a placeholder
  }
}
