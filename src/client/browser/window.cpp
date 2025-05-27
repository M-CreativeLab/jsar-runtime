#include <client/dom/element.hpp>
#include <client/dom/document.hpp>
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

  Window::Window(TrClientContextPerProcess *clientContext)
      : dom::DOMEventTarget(),
        clientContext_(clientContext)
  {
    assert(clientContext_ != nullptr);
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
}
