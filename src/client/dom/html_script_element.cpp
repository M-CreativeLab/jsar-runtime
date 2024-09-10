#include <algorithm>
#include <iostream>

#include "crates/jsar_jsbindings.h"
#include "./html_script_element.hpp"
#include "./document.hpp"
#include "./rendering_context.hpp"

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
    auto renderingContext = ownerDocument.lock()->renderingContext;
    compiledScript = renderingContext->scriptingContext->create(
        baseURI, isClassicScript() ? dom::SourceTextType::Classic : dom::SourceTextType::ESM);
    compiledScript->crossOrigin = crossOrigin == HTMLScriptCrossOrigin::Anonymous ? true : false;
    loadSource();
  }

  void HTMLScriptElement::beforeLoadedCallback()
  {
    scheduleScriptExecution();
  }

#define MAX_SCRIPT_URL_LENGTH 512
  void HTMLScriptElement::loadSource()
  {
    if (src == "" || src.empty())
    {
      compileScript(textContent);
      return;
    }
    else
    {
      auto renderingContext = ownerDocument.lock()->renderingContext;
      {
        char newUrl[MAX_SCRIPT_URL_LENGTH]; // TODO: support the URL without this limit?
        size_t len = create_url_with_path(baseURI.c_str(), src.c_str(), (char **)&newUrl, sizeof(newUrl));
        if (len == 0)
        {
          fprintf(stderr, "Failed to parse the URL: %s\n", src.c_str());
        }
        else
        {
          renderingContext->fetchTextSourceResource(newUrl, [this](const std::string &source)
                                                    { compileScript(source); });
        }
      }
    }
  }

  void HTMLScriptElement::compileScript(const string &source)
  {
    auto renderingContext = ownerDocument.lock()->renderingContext;
    renderingContext->scriptingContext->compile(compiledScript, source);
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
    auto renderingContext = ownerDocument.lock()->renderingContext;
    renderingContext->scriptingContext->evaluate(compiledScript);
    scriptExecutedOnce = true;
    scriptExecutionScheduled = false;
  }
}
