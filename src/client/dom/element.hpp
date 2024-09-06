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
    virtual void createdCallback() {};
    virtual void connectedCallback() {};
    virtual void disconnectedCallback() {};
    virtual void adoptedCallback() {};
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
