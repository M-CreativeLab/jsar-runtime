#pragma once

#include <string>
#include <memory>
#include "./dom_parser.hpp"
#include "./dom_scripting.hpp"
#include "./document.hpp"

namespace dom
{
  class DocumentRenderingContext : public std::enable_shared_from_this<DocumentRenderingContext>
  {
  public:
    DocumentRenderingContext() : scriptingContext(make_shared<DOMScriptingContext>())
    {
    }

  public:
    template <typename DocumentType>
    shared_ptr<DocumentType> start(const std::string &source, DOMParsingType type)
    {
      shared_ptr<DocumentType> document;
      if (type == DOMParsingType::HTML)
        document = make_shared<DocumentType>(true);
      else
        throw std::runtime_error("Unsupported document type");

      document->setSource(source);
      document->open(shared_from_this());
      return document;
    }

  public:
    shared_ptr<DOMScriptingContext> scriptingContext;
  };
}
