#pragma once

#include <string>
#include "./document.hpp"

namespace dom
{
  enum class DOMParsingType
  {
    XML,
    HTML,
    SVG,
  };

  class DOMParser
  {
  public:
    DOMParser();
    ~DOMParser() = default;

    Document parseFromString(const std::string& source, DOMParsingType type);
  };
}
