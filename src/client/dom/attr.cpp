#include "./attr.hpp"
#include "./element.hpp"

namespace dom
{
  using namespace std;

  Attr::Attr(std::string name, std::string initialValue, shared_ptr<Element> ownerElement)
      : Node(NodeType::ATTRIBUTE_NODE, name, ownerElement->getOwnerDocumentReference()),
        name(name),
        value(initialValue),
        specified(true),
        ownerElement(ownerElement)
  {
    string nameSource = name;

    // parse name into `prefix` and `localName`
    size_t colonPos = nameSource.find(':');
    if (colonPos != string::npos)
    {
      prefix = nameSource.substr(0, colonPos);
      localName = nameSource.substr(colonPos + 1);
    }
    else
    {
      localName = nameSource;
    }
  }

  Attr::Attr(pugi::xml_attribute attr, shared_ptr<Element> ownerElement)
      : Attr(attr.name(), attr.value(), ownerElement)
  {
  }

  Attr::Attr(Attr &other)
      : Node(other),
        localName(other.localName),
        name(other.name),
        namespaceURI(other.namespaceURI),
        prefix(other.prefix),
        specified(other.specified),
        value(other.value),
        ownerElement(other.ownerElement)
  {
  }
}
