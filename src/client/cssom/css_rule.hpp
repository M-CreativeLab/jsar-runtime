#pragma once

#include <vector>
#include <memory>
#include <crates/bindings.hpp>

namespace client_cssom
{
  // Forward declarations
  class CSSStyleSheet;

  // Typedefs
  using CSSRuleIndex = size_t;

  enum class CSSRuleType
  {
    kUnknownRule = 0x00,
    kStyleRule,
    kImportRule = 0x03,
    kMediaRule,
    kFontFaceRule,
    kPageRule,
    kKeyframesRule,
    kNamespaceRule = 0x0a,
    kCounterStyleRule,
    kSupportsRule,
    kFontFeatureValuesRule = 0x0e,
  };

  class CSSRule
  {
  public:
    CSSRule() = default;
    virtual ~CSSRule() = default;

  public:
    std::string cssText;
    inline std::shared_ptr<CSSRule> parentRule() const
    {
      return parentRule_.lock();
    }
    inline std::shared_ptr<CSSStyleSheet> parentStyleSheet() const
    {
      return parentStyleSheet_.lock();
    }

  private:
    std::weak_ptr<CSSRule> parentRule_;
    std::weak_ptr<CSSStyleSheet> parentStyleSheet_;
  };

  class CSSRuleList : std::vector<std::shared_ptr<CSSRule>>
  {
    friend class CSSStyleSheet;

  public:
    CSSRuleList() = default;

  public:
    size_t length() const { return size(); }

  public:
    CSSRule &item(CSSRuleIndex index) const { return *at(index); }
    auto begin() { return std::vector<std::shared_ptr<CSSRule>>::begin(); }
    auto end() { return std::vector<std::shared_ptr<CSSRule>>::end(); }
    auto begin() const { return std::vector<std::shared_ptr<CSSRule>>::begin(); }
    auto end() const { return std::vector<std::shared_ptr<CSSRule>>::end(); }

  private:
    CSSRuleIndex insert(crates::css::CSSRuleInner& inner);
  };
}
