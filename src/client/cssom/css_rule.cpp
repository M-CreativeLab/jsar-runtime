#include "./css_rule.hpp"
#include "./rules/all.hpp"

namespace client_cssom
{
  using namespace std;
  using namespace rules;

  CSSRuleIndex CSSRuleList::insert(crates::css::CSSRuleInner &inner)
  {
    if (inner.type() == crates::css::CSSRuleType::Style)
    {
      auto &styleInner = dynamic_cast<crates::css::CSSStyleRuleInner &>(inner);
      push_back(make_shared<CSSStyleRule>(styleInner));
      return size() - 1;
    }
    else if (inner.type() == crates::css::CSSRuleType::Media)
    {
      auto &mediaInner = dynamic_cast<crates::css::CSSMediaRuleInner &>(inner);
      push_back(make_shared<CSSMediaRule>(mediaInner));
      return size() - 1;
    }
    else
    {
      throw runtime_error("Unknown CSS rule type.");
    }
  }
}
