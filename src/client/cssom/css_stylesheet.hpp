#pragma once

#include <string>
#include <optional>

#include "./stylesheet.hpp"
#include "./css_rule.hpp"
#include "./rules/all.hpp"

namespace client_cssom
{
  class CSSStyleSheet : public StyleSheet
  {
  private:
    struct CSSStyleSheetInit
    {
      std::string baseURL;
      bool disabled;
    };

  public:
    CSSStyleSheet(std::optional<CSSStyleSheetInit> init = std::nullopt);

  public:
    void deleteRule();
    void insertRule();
    void replace();
    void replaceSync();

  public:
    std::shared_ptr<rules::CSSImportRule> ownerRule() const { return ownerRule_.lock(); }

  private:
    std::weak_ptr<rules::CSSImportRule> ownerRule_;
  };
}
