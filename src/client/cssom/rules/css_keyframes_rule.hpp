#pragma once

#include <string>
#include "./css_keyframe_rule.hpp"
#include "../css_rule.hpp"
#include "../css_style_declaration.hpp"

namespace client_cssom::rules
{
  class CSSKeyframesRule : public CSSRule
  {
    using CSSRule::CSSRule;

  public:
    std::string name;
    const CSSRuleList &cssRules() const { return *cssRules_; }
    size_t length() const { return cssRules_->length(); }

  public:
    void appendRule(std::string rule);
    /**
     * It deletes the `CSSKeyFrameRule` that matches the specified keyframe selector.
     *
     * @param selector The keyframe selector to delete, which must be: "from" or "to" or a percentage.
     */
    void deleteRule(std::string selector);
    const CSSKeyframeRule &findRule(std::string selector) const;

  private:
    std::unique_ptr<CSSRuleList> cssRules_;
  };
}
