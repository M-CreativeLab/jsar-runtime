#pragma once

#include <functional>
#include <memory>
#include <map>
#include <pugixml/pugixml.hpp>
#include <client/builtin_scene/scene.hpp>

#include "./node.hpp"
#include "./element.hpp"
#include "./html_head_element.hpp"
#include "./html_body_element.hpp"

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
    Document(std::string contentType, std::shared_ptr<BrowsingContext> browsingContext, bool autoConnect = false);
    Document(Document &other);
    virtual ~Document() = default;

  public:
    void setUrl(const string &url);
    void setSource(const string &source);
    void open();
    std::shared_ptr<Element> getElementById(const std::string &id);
    std::vector<shared_ptr<Element>> getElementsByClassName(const std::string &className);
    std::vector<shared_ptr<Element>> getElementsByName(const std::string &name);
    std::vector<shared_ptr<Element>> getElementsByTagName(const std::string &tagName);
    std::shared_ptr<HTMLHeadElement> head();
    std::shared_ptr<HTMLBodyElement> body();

  protected:
    void onInternalUpdated() override;

  private:
    void openInternal();

  public:
    DocumentCompatMode compatMode = DocumentCompatMode::NO_QUIRKS;
    std::string contentType = "text/html";

  public:
    /**
     * The scene to draw contents of the document.
     */
    std::shared_ptr<builtin_scene::Scene> scene;
    std::shared_ptr<BrowsingContext> browsingContext;
    std::shared_ptr<Element> documentElement;

  protected:
    bool autoConnect;
    std::shared_ptr<pugi::xml_document> docInternal;
    std::shared_ptr<HTMLHeadElement> headElement;
    std::shared_ptr<HTMLBodyElement> bodyElement;
    std::vector<std::shared_ptr<Element>> allElementsList;
    std::map<string, std::shared_ptr<Element>> elementMapById;

  private:
    bool isSourceLoaded = false;
    bool shouldOpen = false;
  };

  class XMLDocument : public Document
  {
  public:
    XMLDocument(std::shared_ptr<BrowsingContext> browsingContext, bool autoConnect);
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
    HTMLDocument(std::shared_ptr<BrowsingContext> browsingContext, bool autoConnect);
    ~HTMLDocument() = default;
  };
}
