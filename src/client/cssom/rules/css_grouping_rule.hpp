#pragma once

#include <string>
#include <memory>
#include "../css_rule.hpp"

namespace client_cssom::rules
{
  class CSSGroupingRule : public CSSRule
  {
    using CSSRule::CSSRule;

  public:
    inline CSSRuleList &cssRules() const { return *cssRules_; }

  public:
    size_t insertRule(const std::string &rule, size_t index);
    void deleteRule(size_t index);

  private:
    std::unique_ptr<CSSRuleList> cssRules_;
  };
}
