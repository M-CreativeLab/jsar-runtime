#pragma once

#include <string>
#include "../css_rule.hpp"
#include "../css_style_declaration.hpp"

namespace client_cssom::rules
{
  class CSSKeyframeRule : public CSSRule
  {
    using CSSRule::CSSRule;

  public:
    std::string keyText;
    const CSSStyleDeclaration &style() const { return style_; }

  private:
    CSSStyleDeclaration style_;
  };
}
