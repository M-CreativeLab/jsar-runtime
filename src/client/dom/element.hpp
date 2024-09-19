#pragma once

#include <string>
#include "./node.hpp"

using namespace std;

#define TYPED_ELEMENT_MAP(XX) \
  XX("html", HTMLHtmlElement) \
  XX("head", HTMLHeadElement) \
  XX("body", HTMLBodyElement) \
  XX("img", HTMLImageElement) \
  XX("meta", HTMLMetaElement) \
  XX("script", HTMLScriptElement)

namespace dom
{
  class Attr;
  class Document;
  class Element : public Node
  {
  public:
    /**
     * Create a new `Element` object from a `pugi::xml_node`, which is used to create and initialize an element from the XML parser.
     * 
     * @param node The `pugi::xml_node` object.
     * @param ownerDocument The owner document of the element.
     * @returns The created `Element` object.
     */
    static shared_ptr<Element> CreateElement(pugi::xml_node node, weak_ptr<Document> ownerDocument);

    /**
     * Create a new `Element` object from a tag name, which is used to create an element from scripts such as: `document.createElement('div')`.
     * 
     * @param tagName The tag name of the element.
     * @param ownerDocument The owner document of the element.
     * @returns The created `Element` object.
     */
    static shared_ptr<Element> CreateElement(string namespaceURI, string tagName, weak_ptr<Document> ownerDocument);

  public:
    Element(string tagName, optional<weak_ptr<Document>> ownerDocument);
    Element(pugi::xml_node node, weak_ptr<Document> ownerDocument);
    Element(Element &other);
    ~Element() = default;

  public:
    string getAttribute(const string &name);
    vector<string> getAttributeNames();
    shared_ptr<Attr> getAttributeNode(const string &name);
    bool hasAttribute(const std::string &name);
    bool hasAttributes();
    void setAttribute(const string &name, const string &value);
    void setAttributeNode(shared_ptr<Attr> attr);
    void removeAttribute(const string &name);

  public:
    /**
     * Returns true if the element's tag name is the same as the given tag name ignoring case.
     *
     * @param expectedTagName The expected tag name.
     * @returns True if the element's tag name is the same as the given tag name ignoring case.
     */
    bool is(const string expectedTagName);
    void setId(const string &id);
    void setClassName(const string &className);
    string getInnerHTML();
    void setInnerHTML(const string &html);
    string getOuterHTML();
    void setOuterHTML(const string &html);

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
    virtual void createdCallback() {};
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
    string id;
    string namespaceURI;
    string tagName;
    string localName;
    string className;
    string prefix;
  };
}
