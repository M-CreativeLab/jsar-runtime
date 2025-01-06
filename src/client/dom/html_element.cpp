#include <client/cssom/units.hpp>
#include "./html_element.hpp"
#include "./document-inl.hpp"

namespace dom
{
  using namespace std;

  // TODO: Implement the following methods.
  void HTMLElement::blur() {}
  void HTMLElement::focus() {}
  void HTMLElement::click() {}

  void HTMLElement::createdCallback()
  {
    Element::createdCallback();

    style = client_cssom::CSSStyleDeclaration(getAttribute("style"));
    style.setPropertyIfNotPresent("display", "block");
  }

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
      layoutNode_ = make_shared<crates::jsar::layout::Node>(*layoutAllocator);
      layoutNode_->setStyle(adoptedStyle_);

      // TODO: Append this node to the parent layout node.
      auto parentElement = getParentNodeAs<HTMLElement>();
      if (parentElement != nullptr)
      {
        assert(parentElement->layoutNode_ != nullptr);
        parentElement->layoutNode_->addChild(*layoutNode_);
      }
    }
  }

  void HTMLElement::renderElement(builtin_scene::Scene &scene)
  {
    auto layoutRes = layoutNode_->layout();

#ifdef TR_CLIENT_DOM_VERBOSE
    std::cout << "Rendering element: " << tagName << std::endl;
    std::cout << "  " << "Layout: " << layoutRes << std::endl;
#endif

    if (entity_.has_value())
    {
      using namespace builtin_scene;

      auto &entity = entity_.value();
      auto transform = scene.getComponent<Transform>(entity);
      if (transform != nullptr)
      {
        auto actualWidth = client_cssom::pixelToMeter(layoutRes.width());
        auto actualHeight = client_cssom::pixelToMeter(layoutRes.height());
        transform->setScale({actualWidth, actualHeight, 1.0f});
      }
    }
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

  bool HTMLElement::adoptStyle(client_cssom::CSSStyleDeclaration &style)
  {
    adoptedStyle_ = style;

    // Update the layout node style.
    if (layoutNode_ != nullptr)
    {
      layoutNode_->setStyle(adoptedStyle_);
      return true;
    }
    else
    {
      return false;
    }
  }
}
