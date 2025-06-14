#include <cstring>
#include "./matching.hpp"

namespace client_cssom::selectors
{
  using namespace std;
  using namespace dom;
  using namespace crates;

  bool matchesSelectorList(const css2::selectors::SelectorList &selectors, const shared_ptr<HTMLElement> element)
  {
    MatchingContext context;
    for (const auto &selector : selectors)
    {
      if (matchesSelector(selector, element, context))
        return true;
    }
    return false;
  }

  bool matchesSelector(const css2::selectors::Selector &selector, const shared_ptr<HTMLElement> element, MatchingContext &context)
  {
    assert(!selector.components().empty());
    auto it = selector.components().begin();
    return matchesSelectorComponent(selector, it, element, context);
  }

  // Check if the element matches the specified selector component.
  // NOTE: The component should not be a combinator.
  bool matchesSelectorComponentNonCombinator(const css2::selectors::Component &component,
                                             const shared_ptr<HTMLElement> element,
                                             MatchingContext &context)
  {
    assert(!component.isCombinator());

    if (component.isLocalName())
      return strcasecmp(element->tagName.c_str(), component.name().c_str()) == 0;
    if (component.isId())
      return element->id == component.id();
    if (component.isClass())
      return element->classList().contains(component.name());

    if (component.isPseudoClass())
    {
      if (component.isHover())
        return element->isHovered();
      if (component.isFocus())
        return element->isFocused();
    }

    // Returns false if the above checks did not match.
    return false;
  }

  bool matchesSelectorComponent(const css2::selectors::Selector &selector,
                                std::vector<css2::selectors::Component>::const_iterator &it,
                                const shared_ptr<HTMLElement> element,
                                MatchingContext &context)
  {
    // If we reached the end of the selector, it means that the element matches all the components.
    if (it == selector.components().end())
      return true;

    shared_ptr<HTMLElement> nextElement = element; // The next element to check
    const auto &component = *it;

    if (component.isCombinator())
    {
      switch (component.combinator)
      {
      case css2::selectors::Combinator::kChild:
        if (!element->hasTypedParentNode<HTMLElement>())
          return false;
        nextElement = element->getParentNodeAs<HTMLElement>();
        break;
      case css2::selectors::Combinator::kDescendant:
        if (!element->hasTypedParentNode<HTMLElement>())
          return false;
        else
        {
          const css2::selectors::Component &ancestorComponent = *(++it);
          std::shared_ptr<HTMLElement> maybeAncestorElement = element->getParentNodeAs<HTMLElement>();
          while (true)
          {
            // If we reached the root element, we can stop.
            if (maybeAncestorElement == nullptr)
              return false;

            // If the ancestor element matches the ancestor component, we can go to the next component.
            if (matchesSelectorComponentNonCombinator(ancestorComponent, maybeAncestorElement, context))
            {
              nextElement = maybeAncestorElement;
              break;
            }
            maybeAncestorElement = maybeAncestorElement->getParentNodeAs<HTMLElement>();
          }
        }
        break;
      case css2::selectors::Combinator::kNextSibling:
      case css2::selectors::Combinator::kLaterSibling:
      case css2::selectors::Combinator::kPseudoElement:
      case css2::selectors::Combinator::kSlotAssignment:
      case css2::selectors::Combinator::kPart:
      case css2::selectors::Combinator::kUnknown:
        // TODO
        break;
      }
    }
    else
    {
      // Non-combinator component, we need to check if the element matches the component.
      // - If the element matches the component, we can go to the next component to check until the end of the selector.
      // - If the element does not match the component, we can stop and return false.
      if (!matchesSelectorComponentNonCombinator(component, element, context))
        return false;
    }

    // Go to the next component
    return matchesSelectorComponent(selector,
                                    ++it,
                                    nextElement,
                                    context);
  }
}
