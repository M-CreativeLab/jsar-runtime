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
  {
  }

  const ComputedStyle &Window::getComputedStyle(shared_ptr<dom::Node> elementOrTextNode,
                                                optional<string> pseudoElt) const
  {
    assert(elementOrTextNode != nullptr && elementOrTextNode->isElementOrText() &&
           "The element or text node must not be null and must be an element or text node.");

    shared_ptr<ComputedStyle> computedStyle = document_->styleCache().findStyle(elementOrTextNode);
    if (computedStyle != nullptr)
      return *computedStyle;

    computed::Context context = computed::Context::From(elementOrTextNode);
    computedStyle = document_->styleCache().createStyle(elementOrTextNode, false);
    computedStyle->update(context);

    if (elementOrTextNode->isText())
      return *computedStyle; // If it's a text node, return the computed style directly.

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
    return *computedStyle;
  }

  void Window::applyViewportMeta(const dom::ViewportMeta &viewport_meta)
  {
    bool dimensions_changed = false;

    // Apply width
    if (viewport_meta.device_width)
    {
      // Use device width - keep current values for simplicity
      // In a real implementation, this would get actual device dimensions
    }
    else if (viewport_meta.width)
    {
      float new_width = *viewport_meta.width;
      if (new_width != inner_width_)
      {
        inner_width_ = new_width;
        outer_width_ = new_width;
        device_.setViewportWidth(new_width);
        dimensions_changed = true;
      }
    }

    // Apply height
    if (viewport_meta.device_height)
    {
      // Use device height - keep current values for simplicity
      // In a real implementation, this would get actual device dimensions
    }
    else if (viewport_meta.height)
    {
      float new_height = *viewport_meta.height;
      if (new_height != inner_height_)
      {
        inner_height_ = new_height;
        outer_height_ = new_height;
        device_.setViewportHeight(new_height);
        dimensions_changed = true;
      }
    }

    // TODO: Apply scaling factors (initial-scale, minimum-scale, maximum-scale, user-scalable)
    // These would typically affect devicePixelRatio or zoom level

    if (dimensions_changed)
    {
      // Notify the client context about viewport changes if needed
      // This would trigger a re-layout of the document
      if (client_context_)
      {
        // TODO: Add RPC call to notify about viewport size change
        // client_context_->makeRpcCall("window.viewportChanged", {inner_width_, inner_height_});
      }
    }
  }
}
