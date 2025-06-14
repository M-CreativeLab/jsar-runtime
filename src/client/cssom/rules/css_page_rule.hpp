#pragma once

#include <string>
#include <memory>
#include "../css_rule.hpp"
#include "../css_style_declaration.hpp"

namespace client_cssom::rules
{
  class CSSPageRule : public CSSRule
  {
    using CSSRule::CSSRule;

  public:
    std::string selectorText;
    const CSSStyleDeclaration &style() const
    {
      return style_;
    }

  private:
    CSSStyleDeclaration style_;
  };
}
