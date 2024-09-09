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
    compiledScript = renderingContext->scriptingContext->create(
        baseURI, isClassicScript() ? dom::SourceTextType::Classic : dom::SourceTextType::ESM);
    compiledScript->crossOrigin = crossOrigin == HTMLScriptCrossOrigin::Anonymous ? true : false;

    // Compile
    renderingContext->scriptingContext->compile(compiledScript, textContent);

    // Evaluate if needed
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
    if (compiledScript == nullptr)
      return;
    auto renderingContext = ownerDocument.lock()->renderingContext;
    renderingContext->scriptingContext->evaluate(compiledScript);
    scriptExecutedOnce = true;
  }
}
