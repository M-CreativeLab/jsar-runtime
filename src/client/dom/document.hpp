#pragma once

#include <functional>
#include <memory>
#include "./node.hpp"
#include "./element.hpp"
#include "./html_head_element.hpp"
#include "./html_body_element.hpp"
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

  class BrowsingContext;
  class Document : public Node
  {
  public:
    Document(string contentType, shared_ptr<BrowsingContext> browsingContext, bool autoConnect = false);
    Document(Document &other);
    virtual ~Document() = default;

  public:
    void setUrl(const string &url);
    void setSource(const string &source);
    void open();
    shared_ptr<Element> getElementById(const string &id);
    shared_ptr<HTMLHeadElement> head();
    shared_ptr<HTMLBodyElement> body();

  protected:
    void onInternalUpdated() override;

  private:
    void openInternal();

  public:
    DocumentCompatMode compatMode = DocumentCompatMode::NO_QUIRKS;
    string contentType = "text/html";

  public:
    shared_ptr<BrowsingContext> browsingContext;
    shared_ptr<Element> documentElement;

  protected:
    bool autoConnect;
    shared_ptr<pugi::xml_document> docInternal;
    shared_ptr<HTMLHeadElement> headElement;
    shared_ptr<HTMLBodyElement> bodyElement;

  private:
    bool isSourceLoaded = false;
    bool shouldOpen = false;
  };

  class XMLDocument : public Document
  {
  public:
    XMLDocument(shared_ptr<BrowsingContext> browsingContext, bool autoConnect);
    ~XMLDocument() = default;
  };

  class HTMLDocument : public Document
  {
  public:
    /**
     * Create a new HTMLDocument from a string source.
     * 
     * @param browsingContext The browsing context that the document belongs to.
     * @param autoConnect If true, the document will be automatically to be connected as the DOM root.
     */
    HTMLDocument(shared_ptr<BrowsingContext> browsingContext, bool autoConnect);
    ~HTMLDocument() = default;
  };
}
