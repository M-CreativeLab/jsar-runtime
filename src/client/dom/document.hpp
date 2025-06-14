#pragma once

#include <functional>
#include <memory>
#include <map>
#include <pugixml/pugixml.hpp>
#include <crates/bindings.hpp>
#include <client/animation/document_timeline.hpp>
#include <client/browser/window.hpp>
#include <client/builtin_scene/scene.hpp>
#include <client/cssom/css_stylesheet.hpp>
#include <client/cssom/style_cache.hpp>
#include <client/layout/layout_view.hpp>
#include <client/html/html_head_element.hpp>
#include <client/html/html_body_element.hpp>

#include "./node.hpp"
#include "./node_list.hpp"
#include "./element.hpp"
#include "./text.hpp"
#include "./document_fragment.hpp"

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
    friend class HTMLLinkElement;

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
    /**
     * Create a new document.
     *
     * @param contentType The content type of the document.
     * @param documentType The document type.
     * @param browsingContext The browsing context that the document belongs to.
     * @param autoConnect If true, the document will be automatically to be connected as the DOM root.
     */
    static std::shared_ptr<Document> Make(std::string contentType, DocumentType documentType, std::shared_ptr<BrowsingContext> browsingContext, bool autoConnect = false);

  public:
    /**
     * Don't use this constructor directly, use `Document::Make` instead.
     */
    Document(std::string contentType, DocumentType documentType, std::shared_ptr<BrowsingContext> browsingContext, bool autoConnect = false);
    Document(Document &other) = default;
    virtual ~Document() = default;

  public:
    void setUrl(const string &url);
    void setSource(const string &source, bool isFragment = false);
    void open();
    std::shared_ptr<DocumentFragment> createDocumentFragment();
    std::shared_ptr<Text> createTextNode(const std::string &data);
    std::shared_ptr<Node> importNode(const std::shared_ptr<Node> node, bool deep);
    std::shared_ptr<Element> getElementById(const std::string &id);
    std::vector<shared_ptr<Element>> getElementsByClassName(const std::string &className);
    std::vector<shared_ptr<Element>> getElementsByName(const std::string &name);
    std::vector<shared_ptr<Element>> getElementsByTagName(const std::string &tagName);
    std::shared_ptr<Element> querySelector(const std::string &selectors);
    NodeList<Element> querySelectorAll(const std::string &selectors);

    // Append the stylesheet
    void appendStyleSheet(std::shared_ptr<client_cssom::CSSStyleSheet>);

  protected:
    virtual void onDocumentOpened() {};
    virtual void onStyleSheetsDidChange() {};
    void onNodeAdded(const std::shared_ptr<Node>, bool fast_insert, bool recursive);
    void onNodeRemoved(const std::shared_ptr<Node>, bool recursive);

  private:
    bool isDocument() const override final
    {
      return true;
    }
    void openInternal();
    // Fix the source string to replace invalid tags.
    std::string &fixSource(std::string &source);

  public:
    DocumentCompatMode compatMode = DocumentCompatMode::NO_QUIRKS;
    DocumentType documentType = DocumentType::kHTML;
    std::string contentType = "text/html";
    /**
     * The `defaultView` read-only property of the `Document` interface returns the window object associated with a
     * document, or `null` if none is available.
     */
    inline std::shared_ptr<browser::Window> defaultView() const
    {
      auto ref = default_view_.lock();
      assert(ref != nullptr && "The default view is not set.");
      return ref;
    }

    std::string cookie() const;
    void setCookie(const std::string &new_cookies);

    std::shared_ptr<const DocumentTimeline> timeline() const
    {
      return timeline_;
    }
    const DocumentTimeline &timelineRef() const
    {
      return *timeline_;
    }
    DocumentTimeline &timelineRef()
    {
      return *timeline_;
    }

    /**
     * Get a list of `CSSStyleSheet` objects, for stylesheets explicitly linked into or embedded in a document.
     *
     * @returns The list of `CSSStyleSheet` objects.
     */
    inline const std::vector<std::shared_ptr<client_cssom::CSSStyleSheet>> &styleSheets() const
    {
      return stylesheets_;
    }
    /**
     * Get the style cache for the document.
     *
     * TODO: Will be moved to the `DocumentOrShadowRoot` interface.
     */
    inline client_cssom::StyleCache &styleCache()
    {
      return style_cache_;
    }

  public:
    /**
     * The scene to draw contents of the document.
     */
    std::shared_ptr<builtin_scene::Scene> scene;
    std::shared_ptr<BrowsingContext> browsingContext;
    inline std::shared_ptr<HTMLHeadElement> head() const
    {
      return head_element_;
    }
    inline const HTMLHeadElement &headRef() const
    {
      return *head_element_;
    }
    inline std::shared_ptr<HTMLBodyElement> body() const
    {
      return body_element_;
    }
    inline const HTMLBodyElement &bodyRef() const
    {
      return *body_element_;
    }
    /**
     * The `documentElement` read-only property of the `Document` interface returns the Element that is the root element
     *  of the document (for example, the <html> element for HTML documents).
     */
    inline std::shared_ptr<Element> documentElement() const
    {
      return firstElementChild();
    }
    /**
     * The `Document.firstElementChild` read-only property returns the document's first child Element, or `null` if
     * there are no child elements.
     */
    std::shared_ptr<Element> firstElementChild() const;

  protected:
    bool auto_connect_;
    std::weak_ptr<browser::Window> default_view_;
    std::shared_ptr<pugi::xml_document> doc_internal_;
    std::shared_ptr<HTMLHeadElement> head_element_;
    std::shared_ptr<HTMLBodyElement> body_element_;
    std::vector<std::shared_ptr<Element>> all_elements_list_;
    std::map<string, std::shared_ptr<Element>> element_map_by_id_;

  private:
    bool is_source_loaded_ = false;
    bool should_open_ = false;

    std::map<string, std::string> cookies_;
    std::shared_ptr<DocumentTimeline> timeline_;
    std::vector<std::shared_ptr<client_cssom::CSSStyleSheet>> stylesheets_;
    client_cssom::StyleCache style_cache_;
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
    /**
     * This parses the given markup string and returns a list of nodes.
     *
     * See https://html.spec.whatwg.org/multipage/parsing.html#parsing-html-fragments
     *
     * @param contextElement The context element to parse the markup.
     * @param input The markup string to parse.
     * @returns The list of nodes.
     */
    static std::vector<std::shared_ptr<Node>> ParseFragment(const std::shared_ptr<Element> contextElement,
                                                            const std::string &input);

  public:
    XMLDocument(std::shared_ptr<BrowsingContext> browsingContext, bool autoConnect);
    ~XMLDocument() = default;
  };

  class HTMLDocument : public Document
  {
    friend class DocumentEventDispatcher;

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
    /**
     * This parses the given markup string and returns a list of nodes.
     *
     * See https://html.spec.whatwg.org/multipage/parsing.html#parsing-html-fragments
     *
     * @param contextElement The context element to parse the markup.
     * @param input The markup string to parse.
     * @param allowDeclarativeShadowRoots If true, the declarative shadow roots will be allowed.
     * @returns The list of nodes.
     */
    static std::vector<std::shared_ptr<Node>> ParseFragment(const std::shared_ptr<Element> contextElement,
                                                            const std::string &input,
                                                            bool allowDeclarativeShadowRoots = false);

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
    inline std::shared_ptr<client_layout::LayoutView> layoutView()
    {
      return layout_view_;
    }
    inline std::shared_ptr<const client_layout::LayoutView> layoutView() const
    {
      return layout_view_;
    }
    inline client_layout::LayoutView &layoutViewRef()
    {
      return *layout_view_;
    }
    inline const client_layout::LayoutView &layoutViewRef() const
    {
      return *layout_view_;
    }

    // Returns the current root of text node or element that is dirty, the renderer will draw from this node.
    // Call this function will reset the cached text or element.
    [[nodiscard]] inline std::shared_ptr<Node> getDirtyRootTextOrElement() const
    {
      auto node = dirty_root_text_or_element_.lock();
      dirty_root_text_or_element_.reset();
      return node;
    }
    inline void checkAndSetDirtyRootTextOrElement(std::shared_ptr<Node> node)
    {
      if (node == nullptr)
      {
        dirty_root_text_or_element_.reset();
        return;
      }
      if (dirty_root_text_or_element_.expired())
      {
        dirty_root_text_or_element_ = node;
        return;
      }

      auto current_node = dirty_root_text_or_element_.lock();
      if (node != current_node)
      {
        if (node->depth() < current_node->depth())
        {
          dirty_root_text_or_element_ = node;
        }
        else if (node->depth() == current_node->depth())
        {
          dirty_root_text_or_element_ = node->parentNode.lock();
        }
      }
    }
    // Mark the document cache as dirty, the renderer will draw from the body element.
    inline void invalidateDocumentCache()
    {
      dirty_root_text_or_element_ = body();
    }

    std::optional<builtin_scene::BoundingBox> visualBoundingBox() const;

  public:
    void afterLoadedCallback() override;

  private:
    bool isHTMLDocument() const override final
    {
      return true;
    }
    void onDocumentOpened() override;
    void onStyleSheetsDidChange() override;

    void simulateScrollWithOffset(float offsetX, float offsetY);

  private:
    std::shared_ptr<client_layout::LayoutView> layout_view_;
    mutable std::weak_ptr<Node> dirty_root_text_or_element_;
  };
}
