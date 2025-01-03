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

  void HTMLElement::renderElement(builtin_scene::Scene &scene)
  {
#ifdef TR_CLIENT_DOM_VERBOSE
    std::cout << "Rendering element: " << tagName << std::endl;
#endif
  }

  std::shared_ptr<builtin_scene::Scene> HTMLElement::scene()
  {
    auto ownerDocumentRef = ownerDocument->lock();
    return ownerDocumentRef == nullptr
               ? nullptr
               : ownerDocumentRef->scene;
  }
}
