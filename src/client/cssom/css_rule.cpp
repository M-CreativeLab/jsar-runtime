#include "./css_rule.hpp"
#include "./rules/all.hpp"

namespace client_cssom
{
  using namespace std;
  using namespace rules;
  using namespace crates;

  CSSRuleIndex CSSRuleList::insert(css2::stylesheets::CssRule &inner)
  {
    if (inner.type == css2::stylesheets::CssRuleType::kStyle)
    {
      auto &styleInner = dynamic_cast<css2::stylesheets::StyleRule &>(inner);
      push_back(make_shared<CSSStyleRule>(styleInner));
      return size() - 1;
    }
    else if (inner.type == css2::stylesheets::CssRuleType::kMedia)
    {
      auto &mediaInner = dynamic_cast<css2::stylesheets::MediaRule &>(inner);
      push_back(make_shared<CSSMediaRule>(mediaInner));
      return size() - 1;
    }
    else
    {
      throw runtime_error("Unknown CSS rule type.");
    }
  }
}
