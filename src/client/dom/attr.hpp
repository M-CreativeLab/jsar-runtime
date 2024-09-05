#pragma once

#include <string>
#include "./node.hpp"
#include "./element.hpp"

using namespace std;

namespace dom
{
  class Element;
  class Attr : public Node
  {
  public:
    Attr(pugi::xml_attribute attr, shared_ptr<Element> ownerElement);
    Attr(Attr &other);
    ~Attr() = default;

  public:
    string localName;
    string name;
    string namespaceURI;
    string prefix;
    bool specified;
    string value;
    shared_ptr<Element> ownerElement;

  private:
    shared_ptr<pugi::xml_attribute> attrInternal;
  };
}
