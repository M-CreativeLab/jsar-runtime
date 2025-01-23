#pragma once

#include <string>
#include <optional>

#include "./stylesheet.hpp"
#include "./css_rule.hpp"
#include "./rules/all.hpp"

namespace client_cssom
{
  struct CSSStyleSheetInit
  {
    std::string baseURL;
    bool disabled;
  };

  class CSSStyleSheet : public StyleSheet
  {
  public:
    CSSStyleSheet(std::optional<CSSStyleSheetInit> init = std::nullopt);

  public:
    const CSSRuleList &cssRules() const { return *cssRules_; }
    std::shared_ptr<rules::CSSImportRule> ownerRule() const { return ownerRule_.lock(); }

  public:
    void deleteRule(CSSRuleIndex index);
    CSSRuleIndex insertRule(const std::string &rule, CSSRuleIndex index = 0);
    void replace(const std::string &cssText);
    void replaceSync(const std::string &cssText);

  private:
    CSSStyleSheetInit init_;
    std::unique_ptr<CSSRuleList> cssRules_;
    std::weak_ptr<rules::CSSImportRule> ownerRule_;
  };
}
