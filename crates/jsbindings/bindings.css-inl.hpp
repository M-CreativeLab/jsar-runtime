#pragma once

#include <memory>
#include <string>
#include "./bindings.css.hpp"

namespace crates::css2
{
  namespace properties
  {
    std::shared_ptr<PropertyDeclarationBlock> PropertyDeclarationBlock::ParseStyleDeclaration(const std::string &styleStr)
    {
      auto instance = ::crates::css2::parsing::CSSParser().parseStyleDeclaration(styleStr);
      return std::make_shared<PropertyDeclarationBlock>(instance);
    }
  }
}
