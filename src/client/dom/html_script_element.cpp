#include <iostream>
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
    compiledScriptId = renderingContext->scriptingContext->compile(textContent);

    bool skipScriptExecution = false;
    if (isClassicScript() && defer)
      skipScriptExecution = true;

    if (!skipScriptExecution)
      executeScript();
  }

  void HTMLScriptElement::beforeLoadedCallback()
  {
    if (!scriptExecutedOnce)
      executeScript();
  }

  void HTMLScriptElement::executeScript()
  {
    if (compiledScriptId == 0)
      return;
    auto renderingContext = ownerDocument.lock()->renderingContext;
    renderingContext->scriptingContext->run(compiledScriptId);
    scriptExecutedOnce = true;
  }
}
