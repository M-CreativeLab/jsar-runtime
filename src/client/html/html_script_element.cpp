#include <algorithm>
#include <iostream>
#include <crates/bindings.hpp>
#include <client/dom/document.hpp>
#include <client/dom/browsing_context.hpp>
#include <client/dom/script_execution_manager.hpp>
#include <client/dom/html_script_adapter.hpp>

#include "./html_script_element.hpp"

namespace dom
{
  using namespace std;
  using namespace crates;

  void HTMLScriptElement::createdCallback(bool from_scripting)
  {
    HTMLElement::createdCallback(from_scripting);
    renderable = false;

    // parserInserted is true only for parser-created nodes; false for scripting-created
    parserInserted = !from_scripting;

    if (hasAttribute("src"))
      src = getAttribute("src");
    if (hasAttribute("type"))
      type = getAttribute("type");

    async = hasAttribute("async");
    defer = hasAttribute("defer");
  }

  void HTMLScriptElement::connectedCallback()
  {
    HTMLElement::connectedCallback();

    auto browsingContext = ownerDocument->lock()->browsingContext;
    if (isImportMap())
    {
      auto sourceText = this->textContent();
      if (!browsingContext->updateImportMap(sourceText))
      {
        /**
         * TODO: Follow the spec to handle the error.
         *
         * See: https://developer.mozilla.org/en-US/docs/Web/HTML/Element/script/type/importmap#exceptions
         */
        cerr << "Failed to parse the import map: " << sourceText << endl;
      }
    }
    else if (isClassicScript() || isModuleScript())
    {
      compiledScript = browsingContext->createScript(baseURI, isClassicScript() ? SourceTextType::Classic : SourceTextType::ESM);
      compiledScript->crossOrigin = crossOrigin == HTMLScriptCrossOrigin::Anonymous ? true : false;

      // Use script execution management to handle script execution order
      auto document = ownerDocument->lock();
      if (document && document->isHTMLDocument())
      {
        auto htmlDoc = std::static_pointer_cast<HTMLDocument>(document);

        // Use ScriptExecutionManager
        auto scriptManager = htmlDoc->getScriptExecutionManager();
        if (scriptManager && htmlDoc->isScriptExecutionManagerReady())
        {
          // Use architecture with adapter pattern
          auto scriptAdapter = std::make_shared<HTMLScriptAdapter>(
            std::static_pointer_cast<HTMLScriptElement>(shared_from_this()));
          scriptManager->handleScript(scriptAdapter);
        }
        else
        {
          // Fallback to original behavior
          loadSource();
        }
      }
      else
      {
        // Fallback to original behavior for non-HTML documents
        loadSource();
      }
    }
    // TODO(yorkie): support "speculationrules"?

    // By default, The embedded content is treated as a data block, and won't be processed by the browser.
  }

  void HTMLScriptElement::beforeLoadedCallback()
  {
    // Avoid duplicate execution when ScriptExecutionManager is present.
    auto document = ownerDocument->lock();
    if (document && document->isHTMLDocument())
    {
      auto htmlDoc = std::static_pointer_cast<HTMLDocument>(document);
      if (htmlDoc->isScriptExecutionManagerReady())
      {
        // ScriptExecutionManager will orchestrate loading and execution.
        // Do not schedule here to prevent double execution.
        return;
      }
    }

    // Fallback behavior when manager is not available.
    scheduleScriptExecution();
  }

  void HTMLScriptElement::loadSource()
  {
    if (src == "" || src.empty())
    {
      // For inline scripts, compilation will trigger the loading callback once.
      compileScript(textContent(), false);
      return;
    }
    else
    {
      auto browsingContext = ownerDocument->lock()->browsingContext;
      {
        auto resourceUrl = UrlHelper::CreateUrlStringWithPath(baseURI, src);
        if (resourceUrl == "")
        {
          // signal failure
          auto cb = loadingCallback_;
          loadingCallback_ = nullptr;
          if (cb)
            cb(false);
          // dispatch error event
          dispatchEvent(dom::DOMEventType::Error);
        }
        else
        {
          auto resourceExt = UrlHelper::ParseUrlToModuleExtension(resourceUrl);
          // If the url has extension, check if it is a text source module(JavaScript/TypeScript).
          if (TR_UNLIKELY(!resourceExt.isNone() && !resourceExt.isTextSourceModule()))
          {
            cerr << "The script source is not a text source module: " << resourceUrl << endl
                 << "   src: " << src << endl;
            assert(false && "Only text source module is supported if extension is provided.");
          }

          bool isTypeScript = resourceExt.isTypeScript();

          // TODO: support blocking script loading
          //       requires custom http client implementation
          browsingContext->fetchTextSourceResource(
            resourceUrl,
            [this, isTypeScript](const string &source)
            {
              compileScript(source, isTypeScript);
            },
            [this](const string &errorStr)
            {
              auto cb = loadingCallback_;
              loadingCallback_ = nullptr;
              if (cb)
                cb(false);
              dispatchEvent(dom::DOMEventType::Error);
            });
        }
      }
    }
  }

  void HTMLScriptElement::compileScript(const string &source, bool isTypeScript)
  {
    auto browsingContext = ownerDocument->lock()->browsingContext;
    bool ok = browsingContext->scriptingContext->compile(compiledScript, source, isTypeScript);
    scriptCompiled = ok;

    // Notify loading completion callback if set (for ScriptExecutionManager managed scripts)
    // Ensure the callback is invoked at most once to avoid duplicate execution.
    auto cb = loadingCallback_;
    loadingCallback_ = nullptr;
    if (cb)
    {
      cb(ok);
    }

    if (!ok)
    {
      // Dispatch error for compilation failure
      dispatchEvent(dom::DOMEventType::Error);
      return;
    }
    // Script execution will be handled by ScriptExecutionManager
  }

  void HTMLScriptElement::scheduleScriptExecution()
  {
    if (scriptExecutedOnce)
      return;
    scriptExecutionScheduled = true;

    // Check if the script is already compiled, then schedule the execution by default.
    if (scriptCompiled)
      executeScript();
  }

  void HTMLScriptElement::executeScript()
  {
    if (compiledScript == nullptr || !scriptCompiled)
      return;
    auto browsingContext = ownerDocument->lock()->browsingContext;
    browsingContext->scriptingContext->evaluate(compiledScript);
    scriptExecutedOnce = true;
    scriptExecutionScheduled = false;
    dispatchEvent(dom::DOMEventType::Load);
  }
}
