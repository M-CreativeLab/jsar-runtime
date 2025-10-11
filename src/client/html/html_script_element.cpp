#include <algorithm>
#include <iostream>
#include <limits>
#include <crates/bindings.hpp>
#include <client/dom/document.hpp>
#include <client/dom/browsing_context.hpp>

#include "./html_script_element.hpp"

namespace endor
{
  namespace dom
  {
    using namespace std;
    using namespace crates;

    void HTMLScriptElement::createdCallback(bool from_scripting)
    {
      HTMLElement::createdCallback(from_scripting);
      renderable = false;

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

        // Register classic scripts (non-async, non-defer) with execution queue
        if (isClassicScript() && !async && !defer)
        {
          usesExecutionQueue = true;
          auto scriptElement = dynamic_pointer_cast<HTMLScriptElement>(shared_from_this());
          scriptExecutionId = browsingContext->registerScriptForExecution(scriptElement);
        }

        loadSource();
      }
      // TODO(yorkie): support "speculationrules"?

      // By default, The embedded content is treated as a data block, and won't be processed by the browser.
    }

    void HTMLScriptElement::beforeLoadedCallback()
    {
      scheduleScriptExecution();
    }

    void HTMLScriptElement::loadSource()
    {
      if (src == "" || src.empty())
      {
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
            cerr << "Failed to parse the URL: " << src << endl;
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
            browsingContext->fetchTextSourceResource(resourceUrl, [this, isTypeScript](const string &source)
                                                     { compileScript(source, isTypeScript); });
          }
        }
      }
    }

    void HTMLScriptElement::compileScript(const string &source, bool isTypeScript)
    {
      auto browsingContext = ownerDocument->lock()->browsingContext;
      browsingContext->scriptingContext->compile(compiledScript, source, isTypeScript);
      scriptCompiled = true;

      // After compile
      bool skipScriptExecution = false;
      if (isClassicScript() && defer)
        skipScriptExecution = true;

      if (!skipScriptExecution)
      {
        // If script uses execution queue, let the queue manage execution
        if (usesExecutionQueue)
        {
          browsingContext->tryExecuteNextScript();
        }
        else
        {
          executeScript();
        }
      }
    }

    void HTMLScriptElement::scheduleScriptExecution()
    {
      if (scriptExecutedOnce)
        return;
      scriptExecutionScheduled = true;

      // Check if the script is already compiled, then schedule the execution by default.
      if (scriptCompiled)
      {
        // If script uses execution queue, let the queue manage execution
        if (usesExecutionQueue)
        {
          auto browsingContext = ownerDocument->lock()->browsingContext;
          browsingContext->tryExecuteNextScript();
        }
        else
        {
          executeScript();
        }
      }
    }

    void HTMLScriptElement::executeScript()
    {
      if (compiledScript == nullptr ||
          scriptExecuting ||
          !scriptCompiled)
        return;

      auto browsingContext = ownerDocument->lock()->browsingContext;
      {
        // Mark as executing to prevent re-entrance
        scriptExecuting = true;

        // Execute the script
        browsingContext->scriptingContext->evaluate(compiledScript);

        // Mark as executed
        scriptExecutedOnce = true;
        scriptExecutionScheduled = false;
        scriptExecuting = false;
      }

      // Notify browsing context if this script was using the execution queue
      if (usesExecutionQueue)
        browsingContext->notifyScriptExecutionComplete(scriptExecutionId);

      dispatchEvent(dom::DOMEventType::Load);
    }

    bool HTMLScriptElement::isReadyToExecute() const
    {
      return scriptCompiled && !scriptExecutedOnce && compiledScript != nullptr;
    }

    bool HTMLScriptElement::executeScriptFromQueue()
    {
      if (!scriptExecuting && isReadyToExecute())
      {
        executeScript();
        return true;
      }
      else
      {
        return false;
      }
    }
  }
} // namespace endor
