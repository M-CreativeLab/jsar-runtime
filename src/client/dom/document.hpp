#pragma once

#include <functional>
#include <memory>
#include <map>
#include <pugixml/pugixml.hpp>
#include <client/browser/window.hpp>
#include <client/builtin_scene/scene.hpp>
#include <client/cssom/css_stylesheet.hpp>
#include <client/cssom/style_cache.hpp>
#include <crates/bindings.hpp>

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

  enum class DocumentType
  {
    kHTML,
    kXML,
  };

  class BrowsingContext;
  class Document : public Node
  {
    friend class Node;
    friend class CharacterData;
    friend class Text;
    friend class Element;
    friend class HTMLStyleElement;

  public:
    /**
     * Cast the document to a specific document type.
     *
     * @tparam T The document type to cast to, must be a subclass of `Document`.
     * @param document The document to cast.
     * @returns The casted document.
     */
    template <typename T>
      requires std::is_base_of_v<Document, T>
    static std::shared_ptr<T> As(std::shared_ptr<Document> document);

  public:
    Document(std::string contentType, DocumentType documentType,
             std::shared_ptr<BrowsingContext> browsingContext,
             bool autoConnect = false);
    Document(Document &other);
    virtual ~Document() = default;

  public:
    void setUrl(const string &url);
    void setSource(const string &source);
    void open();
    std::shared_ptr<browser::Window> defaultView();
    std::shared_ptr<Element> getElementById(const std::string &id);
    std::vector<shared_ptr<Element>> getElementsByClassName(const std::string &className);
    std::vector<shared_ptr<Element>> getElementsByName(const std::string &name);
    std::vector<shared_ptr<Element>> getElementsByTagName(const std::string &tagName);
    std::shared_ptr<HTMLHeadElement> head();
    std::shared_ptr<HTMLBodyElement> body();

  protected:
    void connect() override final;
    virtual void onDocumentOpened() {};

  private:
    void openInternal();

  public:
    DocumentCompatMode compatMode = DocumentCompatMode::NO_QUIRKS;
    DocumentType documentType = DocumentType::kHTML;
    std::string contentType = "text/html";
    /**
     * Get a list of `CSSStyleSheet` objects, for stylesheets explicitly linked into or embedded in a document.
     *
     * @returns The list of `CSSStyleSheet` objects.
     */
    inline const std::vector<std::shared_ptr<client_cssom::CSSStyleSheet>> &styleSheets() const
    {
      return styleSheets_;
    }
    /**
     * Get the style cache for the document.
     *
     * TODO: Will be moved to the `DocumentOrShadowRoot` interface.
     */
    inline client_cssom::StyleCache &styleCache()
    {
      return styleCache_;
    }

  public:
    /**
     * The scene to draw contents of the document.
     */
    std::shared_ptr<builtin_scene::Scene> scene;
    std::shared_ptr<BrowsingContext> browsingContext;
    std::shared_ptr<Element> documentElement;

  protected:
    bool autoConnect;
    std::weak_ptr<browser::Window> defaultView_;
    std::shared_ptr<pugi::xml_document> docInternal;
    std::shared_ptr<HTMLHeadElement> headElement;
    std::shared_ptr<HTMLBodyElement> bodyElement;
    std::vector<std::shared_ptr<Element>> allElementsList;
    std::map<string, std::shared_ptr<Element>> elementMapById;

  private:
    bool isSourceLoaded = false;
    bool shouldOpen = false;
    std::vector<std::shared_ptr<client_cssom::CSSStyleSheet>> styleSheets_;
    client_cssom::StyleCache styleCache_;
  };

  class XMLDocument : public Document
  {
  public:
    inline static const DocumentType kDocumentType = DocumentType::kXML;

  public:
    /**
     * This produces an XML serialization of a `Node` node given a flag require well-formed,
     * 
     * @param node The node to serialize.
     * @param wellFormed If true, the serialization will be well-formed.
     * @returns The XML serialization of the node.
     */
    static std::string SerializeFragment(const std::shared_ptr<Node> node, bool wellFormed);

  public:
    XMLDocument(std::shared_ptr<BrowsingContext> browsingContext, bool autoConnect);
    ~XMLDocument() = default;
  };

  class HTMLDocument : public Document
  {
  public:
    inline static const DocumentType kDocumentType = DocumentType::kHTML;
  
  public:
    /**
     * This serializes the children of the node being serialized, not the node itself.
     * 
     * See https://html.spec.whatwg.org/multipage/parsing.html#serialising-html-fragments
     * 
     * @param node The node to serialize.
     * @param serializableShadowRoots If true, the `ShadowRoot` object will be serialized.
     * @param shadowRoots The list of `ShadowRoot` objects.
     * 
     * @todo supports serializing the `ShadowRoot` object.
     */
    static std::string SerializeFragment(const std::shared_ptr<Node> node, bool serializableShadowRoots = false);

  public:
    /**
     * Create a new HTMLDocument from a string source.
     *
     * @param browsingContext The browsing context that the document belongs to.
     * @param autoConnect If true, the document will be automatically to be connected as the DOM root.
     */
    HTMLDocument(std::shared_ptr<BrowsingContext> browsingContext, bool autoConnect);
    ~HTMLDocument() = default;

  public:
    /**
     * Get the layout allocator for the document.
     *
     * @returns The layout allocator.
     */
    inline std::shared_ptr<crates::layout2::Allocator> layoutAllocator() const
    {
      return layoutAllocator_;
    }

  public:
    void load() override;

  private:
    void onDocumentOpened() override;

  private:
    std::shared_ptr<crates::layout2::Allocator> layoutAllocator_;
  };
}
