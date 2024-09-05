#pragma once

#include <memory>
#include "./node.hpp"
#include "./element.hpp"
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
    shared_ptr<Element> getElementById(const string &id);

  public:
    DocumentCompatMode compatMode = DocumentCompatMode::NO_QUIRKS;
    string contentType = "text/html";

  protected:
    shared_ptr<pugi::xml_document> docInternal;
  };

  class XMLDocument : public Document
  {
  public:
    XMLDocument(const std::string &source, bool autoConnect);
    ~XMLDocument() = default;
  };

  class HTMLDocument : public Document
  {
  public:
    /**
     * Create a new HTMLDocument from a string source.
     * 
     * @param source The source of the document.
     * @param autoConnect If true, the document will be automatically to be connected as the DOM root.
     */
    HTMLDocument(const std::string &source, bool autoConnect);
    ~HTMLDocument() = default;
  };
}
