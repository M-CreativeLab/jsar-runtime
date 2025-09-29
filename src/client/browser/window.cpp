#include <client/dom/element.hpp>
#include <client/dom/document.hpp>
#include <client/html/html_meta_element.hpp>
#include <client/cssom/rules/css_style_rule.hpp>
#include <client/cssom/selectors/matching.hpp>
#include <client/cssom/values/computed/context.hpp>
#include <client/html/html_element.hpp>

#include "./window.hpp"

namespace browser
{
  using namespace std;
  using namespace client_cssom;
  using namespace client_cssom::rules;
  using namespace client_cssom::values;

  Window::Window(TrClientContextPerProcess *client_context)
      : dom::DOMEventTarget()
      , client_context_(client_context)
      , navigator_(make_shared<browser::Navigator>(client_context))
  {
  }

  const ComputedStyle &Window::getComputedStyle(shared_ptr<dom::Node> elementOrTextNode,
                                                optional<string> pseudoElt) const
  {
    assert(elementOrTextNode != nullptr && elementOrTextNode->isElementOrText() &&
           "The element or text node must not be null and must be an element or text node.");

    shared_ptr<ComputedStyle> computedStyle = document_->styleCache().findStyle(elementOrTextNode);
    if (computedStyle != nullptr)
    {
      return *computedStyle;
    }
    else
    {
      auto newComputedStyle = createComputedStyle(elementOrTextNode, pseudoElt, true /* writeCache */);
      return *newComputedStyle; // Return the newly created computed style.
    }
  }

  const shared_ptr<client_cssom::ComputedStyle> Window::createComputedStyle(shared_ptr<dom::Node> elementOrTextNode,
                                                                            optional<string> pseudoElt,
                                                                            bool writeCache) const
  {
    assert(elementOrTextNode != nullptr && elementOrTextNode->isElementOrText() &&
           "The element or text node must not be null and must be an element or text node.");

    computed::Context context = computed::Context::From(elementOrTextNode);
    shared_ptr<ComputedStyle> computedStyle = document_->styleCache().createStyle(elementOrTextNode,
                                                                                  false,
                                                                                  writeCache);
    computedStyle->update(context);

    if (elementOrTextNode->isText())
      return computedStyle; // If it's a text node, return the computed style directly.

    // Get the HTML element from the node.
    auto htmlElement = dynamic_pointer_cast<dom::HTMLElement>(elementOrTextNode);
    assert(htmlElement != nullptr && "The node must be an HTMLElement.");

    // Update the style from the stylesheets.
    const auto &stylesheets = elementOrTextNode->getOwnerDocumentChecked().styleSheets();
    for (auto stylesheet : stylesheets)
    {
      for (auto rule : stylesheet->cssRules())
      {
        auto styleRule = dynamic_pointer_cast<CSSStyleRule>(rule);
        if (styleRule != nullptr)
        {
          if (selectors::matchesSelectorList(styleRule->selectors(), htmlElement))
            computedStyle->update(styleRule->style(), context);
        }
        // TODO: handle other types of rules, such as `CSSImportRule`, `CSSMediaRule`, etc.
      }
    }

    // Update the style from the element's inline style.
    auto elementStyle = htmlElement->style();
    computedStyle->update(elementStyle, context); // Override the style from the element's.
    return computedStyle;
  }

  void Window::applyViewportMeta(const dom::ViewportMeta &viewport_meta)
  {
    bool dimensions_changed = false;

    // Apply width using Device class
    if (viewport_meta.device_width)
    {
      float device_width = client_cssom::Device::DeviceWidth;
      if (device_width != inner_width_)
      {
        inner_width_ = device_width;
        outer_width_ = device_width;
        device_.setViewportWidth(device_width, true);
        dimensions_changed = true;
      }
    }
    else if (viewport_meta.width)
    {
      float new_width = *viewport_meta.width;
      if (new_width != inner_width_)
      {
        inner_width_ = new_width;
        outer_width_ = new_width;
        device_.setViewportWidth(new_width, false);
        dimensions_changed = true;
      }
    }

    // Apply height using Device class
    if (viewport_meta.device_height)
    {
      float device_height = client_cssom::Device::DeviceHeight;
      if (device_height != inner_height_)
      {
        inner_height_ = device_height;
        outer_height_ = device_height;
        device_.setViewportHeight(device_height, true);
        dimensions_changed = true;
      }
    }
    else if (viewport_meta.height)
    {
      float new_height = *viewport_meta.height;
      if (new_height != inner_height_)
      {
        inner_height_ = new_height;
        outer_height_ = new_height;
        device_.setViewportHeight(new_height, false);
        dimensions_changed = true;
      }
    }

    // Apply scaling factors using Device class
    if (viewport_meta.initial_scale)
    {
      device_.setInitialScale(*viewport_meta.initial_scale);
    }
    if (viewport_meta.minimum_scale)
    {
      device_.setMinimumScale(*viewport_meta.minimum_scale);
    }
    if (viewport_meta.maximum_scale)
    {
      device_.setMaximumScale(*viewport_meta.maximum_scale);
    }
    if (viewport_meta.user_scalable)
    {
      device_.setUserScalable(*viewport_meta.user_scalable);
    }

    if (dimensions_changed)
    {
      // Notify the client context about viewport changes if needed
      // This would trigger a re-layout of the document
      if (client_context_)
      {
        // TODO(yorkie): Add RPC call to notify about viewport size change
      }
    }
  }

  void Window::configureDocument(std::shared_ptr<dom::Document> document)
  {
    assert(is_document_configured_ == false);
    document_ = document;
    location_ = make_shared<browser::Location>(document_->baseURI);
    is_document_configured_ = true;
  }
}
