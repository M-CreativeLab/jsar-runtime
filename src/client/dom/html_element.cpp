#include <client/per_process.hpp>
#include "./html_element.hpp"
#include "./document.hpp"

namespace dom
{
  using namespace std;

  void HTMLElement::focus()
  {
    // TODO: implement focus
  }

  void HTMLElement::connectedCallback()
  {
    auto builtinScene = TrClientContextPerProcess::Get()->builtinScene;
    if (builtinScene != nullptr)
    {
      builtinScene_ = builtinScene;
      entity_ = builtinScene->createElement(tagName);
    }
  }
}
