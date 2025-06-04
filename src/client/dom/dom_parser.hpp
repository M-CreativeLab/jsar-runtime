#pragma once

#include <string>
#include <memory>
#include "./document.hpp"

namespace dom
{
  enum class DOMParsingType
  {
    XML,
    HTML,
    XSML,
    SVG,
  };
  class DOMParser
  {
  public:
    DOMParser();
    ~DOMParser() = default;

    template <typename DocumentType>
    shared_ptr<DocumentType> parseFromString(const std::string &source, DOMParsingType _type)
    {
      auto document = make_shared<DocumentType>(nullptr, false);
      document->setSource(source);
      return document;
    }
  };
}
