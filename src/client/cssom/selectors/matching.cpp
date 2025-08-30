#include <cstring>
#include <client/dom/document.hpp>
#include "./matching.hpp"

namespace client_cssom::selectors
{
  using namespace std;
  using namespace dom;

  // Helper functions for pseudo-class matching
  bool isRootElement(const shared_ptr<HTMLElement> element)
  {
    try
    {
      const auto &document = element->getOwnerDocumentChecked();
      auto documentElement = document.documentElement();
      return documentElement && documentElement == element;
    }
    catch (...)
    {
      return false;
    }
  }

  bool isFirstChild(const shared_ptr<HTMLElement> element)
  {
    auto parent = element->getParentNode();
    if (!parent)
      return false;

    auto firstChild = parent->firstChild();
    return firstChild && firstChild == element;
  }

  bool isLastChild(const shared_ptr<HTMLElement> element)
  {
    auto parent = element->getParentNode();
    if (!parent)
      return false;

    auto lastChild = parent->lastChild();
    return lastChild && lastChild == element;
  }

  bool isFirstOfType(const shared_ptr<HTMLElement> element)
  {
    auto parent = element->getParentNode();
    if (!parent)
      return false;

    // Check all previous siblings to see if any have the same tag name
    auto currentSibling = element->previousSibling();
    while (currentSibling)
    {
      auto siblingElement = dynamic_pointer_cast<HTMLElement>(currentSibling);
      if (siblingElement && strcasecmp(siblingElement->tagName.c_str(), element->tagName.c_str()) == 0)
        return false; // Found a sibling of the same type before this element
      currentSibling = currentSibling->previousSibling();
    }
    return true;
  }

  bool isLastOfType(const shared_ptr<HTMLElement> element)
  {
    auto parent = element->getParentNode();
    if (!parent)
      return false;

    // Check all next siblings to see if any have the same tag name
    auto currentSibling = element->nextSibling();
    while (currentSibling)
    {
      auto siblingElement = dynamic_pointer_cast<HTMLElement>(currentSibling);
      if (siblingElement && strcasecmp(siblingElement->tagName.c_str(), element->tagName.c_str()) == 0)
        return false; // Found a sibling of the same type after this element
      currentSibling = currentSibling->nextSibling();
    }
    return true;
  }

  bool matchesSelectorList(const SelectorList &selectors, const shared_ptr<HTMLElement> element)
  {
    MatchingContext context;
    for (const auto &selector : selectors)
    {
      if (matchesSelector(selector, element, context))
        return true;
    }
    return false;
  }

  bool matchesSelector(const Selector &selector, const shared_ptr<HTMLElement> element, MatchingContext &context)
  {
    assert(!selector.empty());

    // CSS selectors are matched right-to-left, so we need to start from the end
    // and work backwards through the components
    return matchesSelectorFromEnd(selector, element, context);
  }

  bool matchesSelectorComponentNonCombinator(const Component &component,
                                             const shared_ptr<HTMLElement> element,
                                             MatchingContext &context)
  {
    assert(!component.isCombinator());

    if (component.isUniversal())
      return true; // Universal selector matches any element
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
      if (component.isFirstChild())
        return isFirstChild(element);
      if (component.isLastChild())
        return isLastChild(element);
      if (component.isFirstOfType())
        return isFirstOfType(element);
      if (component.isLastOfType())
        return isLastOfType(element);
      if (component.isWhere())
      {
        // :where() matches if any selector in its argument list matches the element
        if (component.argumentSelectorList())
        {
          return matchesSelectorList(*component.argumentSelectorList(), element);
        }
        return false; // Empty :where() matches nothing
      }
      // TODO: Implement support for :active pseudo-class when element->isActive() is available.
    }

    if (component.isRoot())
      return isRootElement(element);
    // if (component.isEmpty())
    //   return element->isEmpty();

    // Returns false if the above checks did not match.
    return false;
  }

  bool matchesSelectorFromEnd(const Selector &selector,
                              const shared_ptr<HTMLElement> element,
                              MatchingContext &context)
  {
    const auto &components = selector.components();
    if (components.empty())
      return false;

    // Start from the end (rightmost component) and work backwards
    int currentPos = components.size() - 1;
    shared_ptr<HTMLElement> currentElement = element;

    while (currentPos >= 0)
    {
      const auto &component = components[currentPos];

      if (component.isCombinator())
      {
        // Move to the next element based on combinator type
        switch (component.combinator())
        {
        case Combinator::kChild:
          // Child combinator: element must be direct child
          if (!currentElement->hasTypedParentNode<HTMLElement>())
            return false;
          currentElement = currentElement->getParentNodeAs<HTMLElement>();
          break;

        case Combinator::kDescendant:
          // Descendant combinator: find an ancestor that matches next component
          if (!currentElement->hasTypedParentNode<HTMLElement>())
            return false;

          // Get the next component (to the left) that we need to match
          if (currentPos == 0)
            return false; // No component to match

          {
            const auto &ancestorComponent = components[currentPos - 1];
            shared_ptr<HTMLElement> ancestor = currentElement->getParentNodeAs<HTMLElement>();

            // Search up the ancestor chain
            while (ancestor != nullptr)
            {
              if (matchesSelectorComponentNonCombinator(ancestorComponent, ancestor, context))
              {
                currentElement = ancestor;
                currentPos--; // Skip the ancestor component since we matched it
                break;
              }
              ancestor = ancestor->getParentNodeAs<HTMLElement>();
            }

            if (ancestor == nullptr)
              return false; // No matching ancestor found
          }
          break;

        case Combinator::kNextSibling:
        case Combinator::kLaterSibling:
        case Combinator::kPseudoElement:
        case Combinator::kSlotAssignment:
        case Combinator::kPart:
        case Combinator::kUnknown:
          // TODO: Implement these combinators
          return false;
        }
      }
      else
      {
        // Non-combinator component - check if current element matches
        if (!matchesSelectorComponentNonCombinator(component, currentElement, context))
          return false;
      }

      currentPos--;
    }

    return true; // All components matched
  }

  bool matchesSelectorComponent(const Selector &selector,
                                vector<Component>::const_iterator &it,
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
      switch (component.combinator())
      {
      case Combinator::kChild:
        if (!element->hasTypedParentNode<HTMLElement>())
          return false;
        nextElement = element->getParentNodeAs<HTMLElement>();
        break;
      case Combinator::kDescendant:
        if (!element->hasTypedParentNode<HTMLElement>())
          return false;
        else
        {
          const Component &ancestorComponent = *(++it);
          shared_ptr<HTMLElement> maybeAncestorElement = element->getParentNodeAs<HTMLElement>();
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
      case Combinator::kNextSibling:
      case Combinator::kLaterSibling:
      case Combinator::kPseudoElement:
      case Combinator::kSlotAssignment:
      case Combinator::kPart:
      case Combinator::kUnknown:
        // TODO: Implement these combinators
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
