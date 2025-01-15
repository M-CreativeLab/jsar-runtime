#include <client/cssom/units.hpp>
#include "./html_element.hpp"
#include "./document-inl.hpp"

namespace dom
{
  using namespace std;
  using LayoutAllocator = crates::layout::Allocator;
  using LayoutNode = crates::layout::Node;

  // TODO: Implement the following methods.
  void HTMLElement::blur() {}
  void HTMLElement::focus() {}
  void HTMLElement::click() {}

  void HTMLElement::createdCallback()
  {
    Element::createdCallback();

    defaultStyle_.setPropertyIfNotPresent("display", "block");
    defaultStyle_.setPropertyIfNotPresent("width", "auto");
    defaultStyle_.setPropertyIfNotPresent("height", "auto");

    // Create style declaration from the default style & the style attribute.
    string concatedCssText = defaultStyle_.cssText() + ";" + getAttribute("style");
    style = make_shared<client_cssom::CSSStyleDeclaration>(concatedCssText);
  }

  void HTMLElement::connectedCallback()
  {
    // 1. Create the rendering entity.
    useScene([this](builtin_scene::Scene &scene)
             { entity_ = scene.createElement(tagName); });

    // 2. Create the layout node.
    auto layoutAllocator = documentLayoutAllocator();
    if (layoutAllocator != nullptr)
    {
      layoutNode_ = make_shared<LayoutNode>(*layoutAllocator);
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
    auto layoutRes = getLayoutResult(); // This will dispatch the onLayoutSizeChanged internally.
#ifdef TR_CLIENT_DOM_VERBOSE
    cout << "Rendering HTMLElement(" << tagName << "): " << layoutRes << endl;
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

        auto actualX = client_cssom::pixelToMeter(layoutRes.x());
        auto actualY = client_cssom::pixelToMeter(layoutRes.y());
        // Treat the z-index as the z-axis translation in pixels for now.
        // TODO: Support the z-index in the layout system, and convert it to meters.
        auto actualZ = client_cssom::pixelToMeter(adoptedStyle_.getPropertyValueAs<float>("z-index"));
        transform->setTranslation({actualX, actualY, actualZ});
      }

      auto material3d = scene.getComponent<MeshMaterial3d>(entity);
      if (material3d != nullptr)
      {
        // TODO: update material properties?
      }

      auto webContent = scene.getComponent<WebContent>(entity);
      if (webContent != nullptr)
        webContent->setLayout(layoutRes); // Update the layout result into the `WebContent` component.
    }
  }

  void HTMLElement::useScene(const function<void(builtin_scene::Scene &)> &callback)
  {
    auto ownerDocumentRef = ownerDocument->lock();
    if (ownerDocumentRef != nullptr && ownerDocumentRef->scene != nullptr)
      callback(*ownerDocumentRef->scene);
  }

  shared_ptr<LayoutAllocator> HTMLElement::documentLayoutAllocator()
  {
    auto documentRef = Document::As<HTMLDocument>(ownerDocument->lock());
    return documentRef == nullptr
               ? nullptr
               : documentRef->layoutAllocator();
  }

  crates::layout::Layout HTMLElement::getLayoutResult()
  {
    auto layoutRes = layoutNode_->layout();
    if (layoutRes.width() != offsetWidth_ ||
        layoutRes.height() != offsetHeight_)
    {
      offsetWidth_ = layoutRes.width();
      offsetHeight_ = layoutRes.height();
      onLayoutSizeChanged();
    }
    // TODO: support offsetTop, offsetLeft, offsetParent, etc.
    return layoutRes;
  }

  bool HTMLElement::adoptStyle(client_cssom::CSSStyleDeclaration &style)
  {
    adoptedStyle_ = style;
    onAdoptedStyleChanged();

    // Update the layout node style.
    if (layoutNode_ != nullptr)
    {
#ifdef TR_CLIENT_DOM_VERBOSE
      cout << "Adopting style for HTMLElement(" << tagName << "): " << adoptedStyle_ << endl;
#endif
      layoutNode_->setStyle(adoptedStyle_);
      return true;
    }
    else
    {
      return false;
    }
  }
}
