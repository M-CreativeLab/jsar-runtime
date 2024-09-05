#pragma once

#include <string>
#include <memory>
#include "./dom_parser.hpp"
#include "./document.hpp"

namespace dom
{
  class DocumentRenderingContext
  {
  public:
    template <typename DocumentType>
    shared_ptr<DocumentType> start(const std::string &source, DOMParsingType type)
    {
      if (type == DOMParsingType::HTML)
        return make_shared<DocumentType>(source, true);
      // TODO: support XSML and SVG
      throw std::runtime_error("Unsupported document type");
    }
  };
}
