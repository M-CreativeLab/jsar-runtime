#pragma once

#include <functional>
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
    Document(string contentType, shared_ptr<DocumentRenderingContext> renderingContext, bool autoConnect = false);
    Document(Document &other);
    virtual ~Document() = default;

  public:
    void setUrl(const string &url);
    void setSource(const string &source);
    void open();
    shared_ptr<Element> getElementById(const string &id);

  private:
    void openInternal();

  public:
    DocumentCompatMode compatMode = DocumentCompatMode::NO_QUIRKS;
    string contentType = "text/html";

  public:
    shared_ptr<DocumentRenderingContext> renderingContext;

  protected:
    bool autoConnect;
    shared_ptr<pugi::xml_document> docInternal;

  private:
    bool isSourceLoaded = false;
    bool shouldOpen = false;
  };

  class XMLDocument : public Document
  {
  public:
    XMLDocument(shared_ptr<DocumentRenderingContext> renderingContext, bool autoConnect);
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
    HTMLDocument(shared_ptr<DocumentRenderingContext> renderingContext, bool autoConnect);
    ~HTMLDocument() = default;
  };
}
