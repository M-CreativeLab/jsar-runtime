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
  }

  void HTMLScriptElement::connectedCallback()
  {
    auto renderingContext = ownerDocument.lock()->renderingContext;
    auto scriptingContext = renderingContext->scriptingContext;
    scriptingContext->compile(textContent);
    scriptingContext->run();
  }
}
