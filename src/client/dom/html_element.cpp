#include "./html_element.hpp"
#include "./document.hpp"

namespace dom
{
  using namespace std;

  // TODO: Implement the following methods.
  void HTMLElement::blur() {}
  void HTMLElement::focus() {}
  void HTMLElement::click() {}

  void HTMLElement::connectedCallback()
  {
    auto sceneRef = scene();
    if (sceneRef != nullptr)
      entity_ = sceneRef->createElement(tagName);
  }

  std::shared_ptr<builtin_scene::Scene> HTMLElement::scene()
  {
    auto ownerDocumentRef = ownerDocument->lock();
    return ownerDocumentRef == nullptr
               ? nullptr
               : ownerDocumentRef->scene;
  }
}
