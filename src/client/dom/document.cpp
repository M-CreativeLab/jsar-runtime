#include "./parser.hpp"

namespace dom
{
  Document::Document() : Node()
  {
    internalDoc = std::make_shared<pugi::xml_document>();
  }

  Document::Document(Document &other) : Node(other)
  {
    internalDoc = other.internalDoc;
  }

  XMLDocument::XMLDocument(const std::string &source) : Document()
  {
    internalDoc->load_string(source.c_str());
  }

  HTMLDocument::HTMLDocument(const std::string &source) : Document()
  {
    internalDoc->load_string(source.c_str());
  }
}
