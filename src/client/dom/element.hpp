#pragma once

#include <string>
#include "./node.hpp"

using namespace std;

namespace dom
{
  class Attr;
  class Element : public Node
  {
  public:
    static shared_ptr<Element> CreateElement(pugi::xml_node node);

  public:
    Element();
    Element(pugi::xml_node node);
    Element(Element &other);
    ~Element() = default;

  public:
    string getAttribute(const string &name);
    vector<string> getAttributeNames();
    shared_ptr<Attr> getAttributeNode(const string &name);
    bool hasAttribute(const std::string &name);
    bool hasAttributes();

  public:
    void setId(const string &id);
    void setClassName(const string &className);
    string getInnerHTML();
    void setInnerHTML(const string &html);
    string getOuterHTML();
    void setOuterHTML(const string &html);

  public:
    string id;
    string namespaceURI;
    string tagName;
    string localName;
    string className;
    string prefix;
  };
}
