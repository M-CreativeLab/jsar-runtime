#pragma once

#include <string>
#include <memory>
#include "./css_grouping_rule.hpp"
#include "../css_rule.hpp"
#include "../css_style_declaration.hpp"

namespace client_cssom::rules
{
  class CSSConditionRule : public CSSGroupingRule
  {
    using CSSGroupingRule::CSSGroupingRule;

  public:
    const std::string &conditionText() const
    {
      return conditionText_;
    }

  private:
    std::string conditionText_;
  };
}
