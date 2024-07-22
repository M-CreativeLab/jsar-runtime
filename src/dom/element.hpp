#pragma once

#include "./node.hpp"

namespace dom
{
  class Element : public Node
  {
  public:
    Element();
    ~Element() = default;

  public:
    bool hasAttribute(const std::string &name);
    bool hasAttributeNS(const std::string &namespaceURI, const std::string &localName);
    bool hasAttributes();

  public:
    std::string getInnerHTML();
    void setInnerHTML(const std::string &html);
    std::string getOuterHTML();
    void setOuterHTML(const std::string &html);

  public:
    std::string id;
    std::string localName;
    std::string namespaceURI;
    std::string tagName;
  };
}
