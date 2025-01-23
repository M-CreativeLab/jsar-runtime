#pragma once

#include <string>
#include <memory>
#include "../css_rule.hpp"
#include "../css_style_declaration.hpp"

namespace client_cssom::rules
{
  class CSSFontFaceRule : public CSSRule
  {
    using CSSRule::CSSRule;

  public:
    const CSSStyleDeclaration &style() const { return style_; }

  private:
    CSSStyleDeclaration style_;
  };
}
