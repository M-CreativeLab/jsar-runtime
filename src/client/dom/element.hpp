#pragma once

#include <string>
#include "./node.hpp"

using namespace std;

namespace dom
{
  class Attr;
  class Document;
  class Element : public Node
  {
  public:
    static shared_ptr<Element> CreateElement(pugi::xml_node node, weak_ptr<Document> ownerDocument);

  public:
    Element();
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
