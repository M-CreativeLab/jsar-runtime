#include "css_selector_adapter.hpp"

namespace client_cssom::selectors
{
  // SelectorListAdapter implementation
  SelectorListAdapter::SelectorListAdapter(SelectorList selectorList)
      : nativeSelectorList_(std::move(selectorList))
  {
  }

  SelectorListAdapter::SelectorListAdapter(const std::string &selectorText)
  {
    auto parsed = CSSelectorParser::parseSelectors(selectorText);
    if (parsed)
    {
      nativeSelectorList_ = std::move(*parsed);
    }
  }

  SelectorListAdapter::operator std::string() const
  {
    return static_cast<std::string>(nativeSelectorList_);
  }

  std::ostream &operator<<(std::ostream &os, const SelectorListAdapter &list)
  {
    os << static_cast<std::string>(list);
    return os;
  }

  // ComponentAdapter implementation
  ComponentAdapter::ComponentAdapter(const Component &component)
      : nativeComponent_(component)
  {
  }

  crates::css2::selectors::Combinator ComponentAdapter::combinator() const
  {
    switch (nativeComponent_.combinator())
    {
    case Combinator::kChild:
      return crates::css2::selectors::Combinator::kChild;
    case Combinator::kDescendant:
      return crates::css2::selectors::Combinator::kDescendant;
    case Combinator::kNextSibling:
      return crates::css2::selectors::Combinator::kNextSibling;
    case Combinator::kLaterSibling:
      return crates::css2::selectors::Combinator::kLaterSibling;
    case Combinator::kPseudoElement:
      return crates::css2::selectors::Combinator::kPseudoElement;
    case Combinator::kSlotAssignment:
      return crates::css2::selectors::Combinator::kSlotAssignment;
    case Combinator::kPart:
      return crates::css2::selectors::Combinator::kPart;
    default:
      return crates::css2::selectors::Combinator::kUnknown;
    }
  }

  // SelectorAdapter implementation
  SelectorAdapter::SelectorAdapter(const Selector &selector)
      : nativeSelector_(selector)
  {
  }

  std::vector<ComponentAdapter> SelectorAdapter::components() const
  {
    std::vector<ComponentAdapter> adapters;
    for (const auto &component : nativeSelector_.components())
    {
      adapters.emplace_back(component);
    }
    return adapters;
  }
}