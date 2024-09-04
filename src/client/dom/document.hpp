#pragma once

#include <memory>
#include "./node.hpp"
#include "pugixml/pugixml.hpp"

using namespace std;
using namespace pugi;

namespace dom
{
  enum class DocumentCompatMode
  {
    NO_QUIRKS = 0, // Standards
    QUIRKS,
    LIMITED_QUIRKS,
  };

  class Document : public Node
  {
  public:
    Document(string contentType = "text/html");
    Document(Document &other);
    ~Document() = default;

  public:
    DocumentCompatMode compatMode = DocumentCompatMode::NO_QUIRKS;
    string contentType = "text/html";

  protected:
    shared_ptr<pugi::xml_document> docInternal;
  };

  class XMLDocument : public Document
  {
  public:
    XMLDocument(const std::string &source);
    ~XMLDocument() = default;
  };

  class HTMLDocument : public Document
  {
  public:
    HTMLDocument(const std::string &source);
    ~HTMLDocument() = default;
  };
}
