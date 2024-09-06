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

  class DocumentRenderingContext;
  class Document : public Node
  {
  public:
    Document(string contentType = "text/html", bool autoConnect = false);
    Document(Document &other);
    ~Document() = default;

  public:
    void setSource(const string &source);
    void open(shared_ptr<DocumentRenderingContext> renderingContext);
    shared_ptr<Element> getElementById(const string &id);

  public:
    DocumentCompatMode compatMode = DocumentCompatMode::NO_QUIRKS;
    string contentType = "text/html";

  public:
    shared_ptr<DocumentRenderingContext> renderingContext;

  protected:
    bool autoConnect;
    shared_ptr<pugi::xml_document> docInternal;
  };

  class XMLDocument : public Document
  {
  public:
    XMLDocument(bool autoConnect);
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
    HTMLDocument(bool autoConnect);
    ~HTMLDocument() = default;
  };
}
