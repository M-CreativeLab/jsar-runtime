#include "./css_stylesheet.hpp"

namespace client_cssom
{
  using namespace std;

  CSSStyleSheet::CSSStyleSheet(optional<CSSStyleSheetInit> init)
      : StyleSheet(),
        init_(init.value_or(CSSStyleSheetInit{})),
        cssRules_(make_unique<CSSRuleList>())
  {
  }

  void CSSStyleSheet::deleteRule(CSSRuleIndex index)
  {
    cssRules_->erase(cssRules_->begin() + index);
  }

  CSSRuleIndex CSSStyleSheet::insertRule(const string &ruleText, CSSRuleIndex index)
  {
    return index;
  }

  void CSSStyleSheet::replace(const string &cssText)
  {
    replaceSync(cssText);
  }

  void CSSStyleSheet::replaceSync(const string &cssText)
  {
    cout << "Parsing CSS text: " << cssText << endl;
  }
}
