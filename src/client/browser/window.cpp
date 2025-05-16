#include <client/dom/element.hpp>
#include <client/dom/document.hpp>
#include <client/cssom/rules/css_style_rule.hpp>
#include <client/cssom/selectors/matching.hpp>
#include <client/html/html_element.hpp>

#include "./window.hpp"

namespace browser
{
  using namespace std;
  using namespace client_cssom;
  using namespace client_cssom::rules;

  Window::Window(TrClientContextPerProcess *clientContext)
      : dom::DOMEventTarget(),
        clientContext_(clientContext)
  {
    assert(clientContext_ != nullptr);
  }

  const ComputedStyle &Window::getComputedStyle(shared_ptr<dom::Element> element, optional<string> pseudoElt) const
  {
    auto htmlElement = dynamic_pointer_cast<dom::HTMLElement>(element);
    if (TR_UNLIKELY(htmlElement == nullptr))
      throw invalid_argument("The element must be an HTMLElement");

    shared_ptr<ComputedStyle> computedStyle = document_->styleCache().findStyle(htmlElement);
    if (computedStyle != nullptr)
      return *computedStyle;

    computedStyle = document_->styleCache().createStyle(htmlElement, false);

    // Initial the style from the element's default style.
    computedStyle->update(htmlElement->defaultStyleRef());

    // Update the style from the stylesheets.
    const auto &stylesheets = htmlElement->getOwnerDocumentChecked().styleSheets();
    for (auto stylesheet : stylesheets)
    {
      for (auto rule : stylesheet->cssRules())
      {
        auto styleRule = dynamic_pointer_cast<CSSStyleRule>(rule);
        if (styleRule != nullptr)
        {
          if (selectors::matchesSelectorList(styleRule->selectors(), htmlElement))
            computedStyle->update(styleRule->style());
        }
        // TODO: handle other types of rules, such as `CSSImportRule`, `CSSMediaRule`, etc.
      }
    }

    // Update the style from the element's inline style.
    auto elementStyle = htmlElement->style();
    computedStyle->update(elementStyle); // Override the style from the element's.
    return *computedStyle;
  }
}
