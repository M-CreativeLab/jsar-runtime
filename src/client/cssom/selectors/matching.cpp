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
    for (const auto &component : selector.components())
    {
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
    }
    return true;
  }
}
