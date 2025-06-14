#pragma once

#include <string>
#include <memory>
#include <crates/bindings.hpp>

#include "./css_condition_rule.hpp"
#include "../media.hpp"
#include "../css_rule.hpp"
#include "../css_style_declaration.hpp"

namespace client_cssom::rules
{
  class CSSMediaRule : public CSSConditionRule
  {
    using CSSConditionRule::CSSConditionRule;

  public:
    CSSMediaRule(crates::css2::stylesheets::MediaRule &inner)
        : CSSConditionRule()
    {
    }

  public:
    const MediaList &media() const
    {
      return *media_;
    }

  private:
    std::unique_ptr<MediaList> media_;
  };
}
