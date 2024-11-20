#include <algorithm>
#include <iostream>

#include "crates/jsar_jsbindings.h"
#include "./html_script_element.hpp"
#include "./document.hpp"
#include "./browsing_context.hpp"

namespace dom
{
  void HTMLScriptElement::createdCallback()
  {
    if (hasAttribute("src"))
      src = getAttribute("src");
    if (hasAttribute("type"))
      type = getAttribute("type");

    async = hasAttribute("async");
    defer = hasAttribute("defer");
  }

  void HTMLScriptElement::connectedCallback()
  {
    auto browsingContext = ownerDocument->lock()->browsingContext;
    if (isImportMap())
    {
      if (!browsingContext->updateImportMap(textContent))
      {
        /**
         * TODO: Follow the spec to handle the error.
         *
         * See: https://developer.mozilla.org/en-US/docs/Web/HTML/Element/script/type/importmap#exceptions
         */
        std::cerr << "Failed to parse the import map: " << textContent << std::endl;
      }
    }
    else
    {
      compiledScript = browsingContext->createScript(baseURI, isClassicScript() ? dom::SourceTextType::Classic : dom::SourceTextType::ESM);
      compiledScript->crossOrigin = crossOrigin == HTMLScriptCrossOrigin::Anonymous ? true : false;
      loadSource();
    }
  }

  void HTMLScriptElement::beforeLoadedCallback()
  {
    scheduleScriptExecution();
  }

  void HTMLScriptElement::loadSource()
  {
    if (src == "" || src.empty())
    {
      compileScript(textContent, false);
      return;
    }
    else
    {
      auto browsingContext = ownerDocument->lock()->browsingContext;
      {
        auto resourceUrl = crates::jsar::UrlHelper::CreateUrlStringWithPath(baseURI, src);
        if (resourceUrl == "")
        {
          std::cerr << "Failed to parse the URL: " << src << std::endl;
        }
        else
        {
          auto resourceExt = crates::jsar::UrlHelper::ParseUrlToModuleExtension(resourceUrl);
          assert(resourceExt.isTextSourceModule());
          bool isTypeScript = resourceExt.isTypeScript();

          browsingContext->fetchTextSourceResource(resourceUrl, [this, isTypeScript](const std::string &source)
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
      executeScript();
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
