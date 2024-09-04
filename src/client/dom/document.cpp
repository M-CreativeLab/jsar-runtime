#include "./parser.hpp"

namespace dom
{
  Document::Document() : Node()
  {
    docInternal = std::make_shared<pugi::xml_document>();
  }

  Document::Document(Document &other) : Node(other)
  {
    docInternal = other.docInternal;
  }

  XMLDocument::XMLDocument(const std::string &source) : Document()
  {
    auto r = docInternal->load_string(source.c_str());
    if (r.status != pugi::xml_parse_status::status_ok)
      throw std::runtime_error("Failed to parse XML document: " + std::string(r.description()));
    resetInternal(docInternal.get());
  }

  HTMLDocument::HTMLDocument(const std::string &source) : Document()
  {
    auto r = docInternal->load_string(source.c_str());
    if (r.status != pugi::xml_parse_status::status_ok)
      throw std::runtime_error("Failed to parse HTML document: " + std::string(r.description()));
    resetInternal(docInternal.get());
  }
}
