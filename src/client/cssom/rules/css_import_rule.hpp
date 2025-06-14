#pragma once

#include <string>
#include <memory>
#include "../css_rule.hpp"

namespace client_cssom::rules
{
  class CSSImportRule : public CSSRule
  {
    using CSSRule::CSSRule;

  public:
    inline const std::string &href() const
    {
      return href_;
    }
    inline const std::string &layerName() const
    {
      return layerName_;
    }
    inline const std::optional<std::string> &supportsText() const
    {
      return supportsText_;
    }

  private:
    std::string href_;
    std::string layerName_;
    std::optional<std::string> supportsText_;
  };
}
