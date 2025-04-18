#pragma once

#include <string>
#include <memory>

#include "./node.hpp"
#include "./element.hpp"

using namespace std;

namespace dom
{
  class Element;
  class Attr : public Node
  {
  public:
    inline static shared_ptr<Attr> Make(shared_ptr<Element> ownerElement, std::string name, std::string initialValue)
    {
      return make_shared<Attr>(name, initialValue, ownerElement);
    }
    inline static shared_ptr<Attr> Make(shared_ptr<Element> ownerElement, pugi::xml_attribute attr)
    {
      assert(attr.name() != nullptr && "The attribute name is null.");
      assert(attr.value() != nullptr && "The attribute value is null.");
      return make_shared<Attr>(attr, ownerElement);
    }

  public:
    Attr(std::string name, std::string initialValue, shared_ptr<Element> ownerElement);
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
  };
}
