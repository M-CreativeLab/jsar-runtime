#include <cstring>
#include "./matching.hpp"

namespace client_cssom::selectors
{
  using namespace std;
  using namespace dom;
  using namespace crates::css;

  bool matchesSelectorList(const CSSSelectorList &selectors, const shared_ptr<HTMLElement> element)
  {
    for (const auto &selector : selectors)
    {
      if (matchesSelector(selector, element))
        return true;
    }
    return false;
  }

  bool matchesSelector(const CSSSelector &selector, const shared_ptr<HTMLElement> element)
  {
    assert(!selector.components().empty());
    auto it = selector.components().begin();
    return matchesSelectorComponent(selector, it, element);
  }

  bool matchesSelectorComponent(const CSSSelector &selector, std::vector<CSSSelectorComponent>::const_iterator &it,
                                const shared_ptr<HTMLElement> element)
  {
    // If we reached the end of the selector, it means that the element matches all the components.
    if (it == selector.components().end())
      return true;

    shared_ptr<HTMLElement> nextElement = element;
    const auto &component = *it;
    if (component.isLocalName())
    {
      if (strcasecmp(element->tagName.c_str(), component.name().c_str()) != 0)
        return false;
    }
    else if (component.isId())
    {
      if (element->id != component.id())
        return false;
    }
    else if (component.isClass())
    {
      if (!element->classList().contains(component.name()))
        return false;
    }
    else if (component.isCombinator())
    {
      switch (component.combinator())
      {
      case CSSSelectorCombinator::Child:
        if (!element->hasTypedParentNode<HTMLElement>())
          return false;
        nextElement = element->getParentNodeAs<HTMLElement>();
        break;
      case CSSSelectorCombinator::Descendant:
      case CSSSelectorCombinator::NextSibling:
      case CSSSelectorCombinator::LaterSibling:
      case CSSSelectorCombinator::PseudoElement:
      case CSSSelectorCombinator::SlotAssignment:
      case CSSSelectorCombinator::Part:
        // TODO
        break;
      }
    }
    return matchesSelectorComponent(selector, ++it, nextElement);
  }
}
