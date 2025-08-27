#pragma once

#include "css_selector_parser.hpp"
#include <crates/jsbindings/bindings.hpp>

namespace client_cssom::selectors
{
  /**
   * Adapter class to make our native C++ SelectorList compatible 
   * with the existing crates::css2::selectors::SelectorList interface
   */
  class SelectorListAdapter
  {
  public:
    SelectorListAdapter(SelectorList selectorList);
    SelectorListAdapter(const std::string &selectorText);

    // Provide the same interface as crates::css2::selectors::SelectorList
    operator std::string() const;
    friend std::ostream &operator<<(std::ostream &os, const SelectorListAdapter &list);

    // Iterator interface for compatibility with existing matching code
    auto begin() const
    {
      return nativeSelectorList_.begin();
    }
    auto end() const
    {
      return nativeSelectorList_.end();
    }
    auto begin()
    {
      return nativeSelectorList_.begin();
    }
    auto end()
    {
      return nativeSelectorList_.end();
    }

    size_t size() const
    {
      return nativeSelectorList_.size();
    }
    bool empty() const
    {
      return nativeSelectorList_.empty();
    }

    // Access to the native implementation
    const SelectorList &native() const
    {
      return nativeSelectorList_;
    }

  private:
    SelectorList nativeSelectorList_;
  };

  /**
   * Adapter class to make our native C++ Component compatible 
   * with the existing crates::css2::selectors::Component interface
   */
  class ComponentAdapter
  {
  public:
    ComponentAdapter(const Component &component);

    // Provide compatibility with crates::css2::selectors::Component interface
    bool isLocalName() const
    {
      return nativeComponent_.isLocalName();
    }
    bool isId() const
    {
      return nativeComponent_.isId();
    }
    bool isClass() const
    {
      return nativeComponent_.isClass();
    }
    bool isRoot() const
    {
      return nativeComponent_.isRoot();
    }
    bool isEmpty() const
    {
      return nativeComponent_.isEmpty();
    }
    bool isHost() const
    {
      return nativeComponent_.isHost();
    }
    bool isPseudoElement() const
    {
      return nativeComponent_.isPseudoElement();
    }
    bool isPseudoClass() const
    {
      return nativeComponent_.isPseudoClass();
    }
    bool isCombinator() const
    {
      return nativeComponent_.isCombinator();
    }

    bool isHover() const
    {
      return nativeComponent_.isHover();
    }
    bool isActive() const
    {
      return nativeComponent_.isActive();
    }
    bool isFocus() const
    {
      return nativeComponent_.isFocus();
    }

    const std::string &name() const
    {
      return nativeComponent_.name();
    }
    const std::string &id() const
    {
      return nativeComponent_.id();
    }
    const std::string &className() const
    {
      return nativeComponent_.className();
    }

    // Convert our native combinator to the expected type
    crates::css2::selectors::Combinator combinator() const;

    operator std::string() const
    {
      return static_cast<std::string>(nativeComponent_);
    }

  private:
    const Component &nativeComponent_;
  };

  /**
   * Adapter class to make our native C++ Selector compatible 
   * with the existing crates::css2::selectors::Selector interface
   */
  class SelectorAdapter
  {
  public:
    SelectorAdapter(const Selector &selector);

    // Provide access to components through adapters
    std::vector<ComponentAdapter> components() const;
    bool empty() const
    {
      return nativeSelector_.empty();
    }
    size_t size() const
    {
      return nativeSelector_.size();
    }

    operator std::string() const
    {
      return static_cast<std::string>(nativeSelector_);
    }

  private:
    const Selector &nativeSelector_;
  };
}