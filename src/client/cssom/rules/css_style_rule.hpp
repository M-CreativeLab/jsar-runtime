#pragma once

#include <string>
#include <memory>

#include "./css_grouping_rule.hpp"
#include "../css_style_declaration.hpp"

namespace client_cssom::rules
{
  class CSSStyleRule : public CSSGroupingRule
  {
    using CSSGroupingRule::CSSGroupingRule;

  public:
    std::string selectorText;
    const CSSStyleDeclaration &style() const { return style_; }

  private:
    CSSStyleDeclaration style_;
  };
}
