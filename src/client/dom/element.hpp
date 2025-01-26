#pragma once

#include <string>
#include <client/cssom/box_bounding.hpp>
#include "./node.hpp"

#define TYPED_ELEMENT_MAP(XX)     \
  XX("audio", HTMLAudioElement)   \
  XX("body", HTMLBodyElement)     \
  XX("canvas", HTMLCanvasElement) \
  XX("div", HTMLDivElement)       \
  XX("head", HTMLHeadElement)     \
  XX("html", HTMLHtmlElement)     \
  XX("img", HTMLImageElement)     \
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
    std::string getAttribute(const std::string &name);
    std::vector<std::string> getAttributeNames();
    std::shared_ptr<Attr> getAttributeNode(const std::string &name);
    bool hasAttribute(const std::string &name);
    bool hasAttributes();
    void setAttribute(const std::string &name, const std::string &value);
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
    void setClassName(const std::string &className);
    std::string getInnerHTML();
    void setInnerHTML(const std::string &html);
    std::string getOuterHTML();
    void setOuterHTML(const std::string &html);

  protected: // Element lifecycle callbacks
    virtual void connect() override
    {
      connectedCallback();
      Node::connect();
    }
    virtual void load() override
    {
      beforeLoadedCallback();
      Node::load();
      afterLoadedCallback();
    }
    /**
     * When the element is created.
     */
    virtual void createdCallback();
    /**
     * When the element is connected to DOM.
     */
    virtual void connectedCallback() {};
    /**
     * When the element is disconnected from DOM.
     */
    virtual void disconnectedCallback() {};
    /**
     * When the element is moved to a new document each time.
     */
    virtual void adoptedCallback() {};
    /**
     * Before the element is loaded.
     */
    virtual void beforeLoadedCallback() {};
    /**
     * After the element is loaded.
     */
    virtual void afterLoadedCallback() {};
    /**
     * When the element's attribute is changed.
     *
     * @param name The name of the attribute.
     * @param oldValue The old value of the attribute.
     * @param newValue The new value of the attribute.
     */
    virtual void attributeChangedCallback(const string &name, const string &oldValue, const string &newValue) {};

  public:
    std::string id;
    std::string namespaceURI;
    std::string tagName;
    std::string localName;
    std::string className;
    std::string prefix;
  };
}
