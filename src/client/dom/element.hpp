#pragma once

#include <string>
#include <unordered_map>
#include <client/cssom/box_bounding.hpp>

#include "./node.hpp"
#include "./dom_token_list.hpp"

#define TYPED_ELEMENT_MAP(XX)     \
  XX("audio", HTMLAudioElement)   \
  XX("body", HTMLBodyElement)     \
  XX("canvas", HTMLCanvasElement) \
  XX("div", HTMLDivElement)       \
  XX("head", HTMLHeadElement)     \
  XX("html", HTMLHtmlElement)     \
  XX("img", HTMLImageElement)     \
  XX("link", HTMLLinkElement)     \
  XX("meta", HTMLMetaElement)     \
  XX("h1", HTMLHeadingElement)    \
  XX("h2", HTMLHeadingElement)    \
  XX("h3", HTMLHeadingElement)    \
  XX("h4", HTMLHeadingElement)    \
  XX("h5", HTMLHeadingElement)    \
  XX("h6", HTMLHeadingElement)    \
  XX("p", HTMLParagraphElement)   \
  XX("script", HTMLScriptElement) \
  XX("span", HTMLSpanElement)     \
  XX("style", HTMLStyleElement)   \
  XX("video", HTMLVideoElement)

namespace dom
{
  class Attr;
  class Document;
  class Element : public Node,
                  virtual public client_cssom::BoxBounding
  {
  public:
    /**
     * Create a new `Element` object from a `pugi::xml_node`, which is used to create and initialize an element from the XML parser.
     *
     * @param node The `pugi::xml_node` object.
     * @param ownerDocument The owner document of the element.
     * @returns The created `Element` object.
     */
    static shared_ptr<Element> CreateElement(pugi::xml_node node, std::shared_ptr<Document> ownerDocument);

    /**
     * Create a new `Element` object from a tag name, which is used to create an element from scripts such as: `document.createElement('div')`.
     *
     * @param tagName The tag name of the element.
     * @param ownerDocument The owner document of the element.
     * @returns The created `Element` object.
     */
    static shared_ptr<Element> CreateElement(std::string namespaceURI, std::string tagName, std::shared_ptr<Document> ownerDocument);

  public:
    Element(std::string tagName, std::optional<std::shared_ptr<Document>> ownerDocument);
    Element(pugi::xml_node node, std::shared_ptr<Document> ownerDocument);
    Element(Element &other);
    ~Element() = default;

  public:
    std::string getAttribute(const std::string &name) const;
    std::vector<std::string> getAttributeNames() const;
    std::shared_ptr<Attr> getAttributeNode(const std::string &name) const;
    Attr &getAttributeNodeChecked(const std::string &name) const;
    bool hasAttribute(const std::string &name) const;
    bool hasAttributes() const;
    /**
     * Sets the attribute value of the element.
     * 
     * @param name The name of the attribute.
     * @param value The value of the attribute.
     * @param notify Whether to notify the attribute change.
     */
    void setAttribute(const std::string &name, const std::string &value, bool notify = true);
    void setAttributeNode(std::shared_ptr<Attr> attr);
    void removeAttribute(const std::string &name);

  public:
    /**
     * Returns true if the element's tag name is the same as the given tag name ignoring case.
     *
     * @param expectedTagName The expected tag name.
     * @returns True if the element's tag name is the same as the given tag name ignoring case.
     */
    bool is(const std::string expectedTagName);
    void setId(const std::string &id);
    std::string getInnerHTML();
    void setInnerHTML(const std::string &markup);
    std::string getOuterHTML();
    void setOuterHTML(const std::string &markup);

  protected: // Element lifecycle callbacks
    /**
     * When the element is created each time.
     */
    virtual void createdCallback();
    /**
     * When the element is moved to a new document each time.
     */
    virtual void adoptedCallback();
    /**
     * When the element's attribute is changed.
     *
     * @param name The name of the attribute.
     * @param oldValue The old value of the attribute.
     * @param newValue The new value of the attribute.
     */
    virtual void attributeChangedCallback(const string &name, const string &oldValue, const string &newValue);

  public:
    std::string id;
    std::string namespaceURI;
    std::string tagName;
    std::string localName;
    std::string prefix;
    inline const std::string &className() const { return classList_.value(); }
    inline void setClassName(const std::string &className)
    {
      classList_ = DOMTokenList(classList_, className);
    }
    inline DOMTokenList &classList() { return classList_; }

  protected:
    DOMTokenList classList_;
    std::unordered_map<std::string, std::shared_ptr<Attr>> attributeNodes_;
  };
}
