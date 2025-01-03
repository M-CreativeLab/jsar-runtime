#include "./html_element.hpp"
#include "./document-inl.hpp"

namespace dom
{
  using namespace std;

  // TODO: Implement the following methods.
  void HTMLElement::blur() {}
  void HTMLElement::focus() {}
  void HTMLElement::click() {}

  void HTMLElement::connectedCallback()
  {
    // 1. Create the rendering entity.
    auto sceneRef = scene();
    if (sceneRef != nullptr)
      entity_ = sceneRef->createElement(tagName);

    // 2. Create the layout node.
    auto layoutAllocator = documentLayoutAllocator();
    if (layoutAllocator != nullptr)
    {
      layoutNode_ = std::make_shared<crates::jsar::layout::Node>(*layoutAllocator);
      // TODO: Append this node to the parent layout node.
    }
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

  std::shared_ptr<crates::jsar::layout::Allocator> HTMLElement::documentLayoutAllocator()
  {
    auto documentRef = Document::As<HTMLDocument>(ownerDocument->lock());
    return documentRef == nullptr
               ? nullptr
               : documentRef->layoutAllocator();
  }
}
