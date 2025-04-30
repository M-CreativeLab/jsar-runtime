#pragma once

#include <unordered_map>
#include <string>

#include "./css_style_declaration.hpp"

namespace client_cssom
{
  class ComputedStyle : std::unordered_map<std::string, std::string>
  {
  public:
    ComputedStyle() = default;
    ComputedStyle(const CSSStyleDeclaration &);

  private:
    void resolve(const CSSStyleDeclaration &);
  };
}
