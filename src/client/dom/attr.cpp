#include "./attr.hpp"
#include "./element.hpp"

namespace dom
{
  Attr::Attr(pugi::xml_attribute attr, shared_ptr<Element> ownerElement)
      : Node(),
        name(attr.name()),
        value(attr.value()),
        specified(true),
        ownerElement(ownerElement),
        attrInternal(make_shared<pugi::xml_attribute>(attr))
  {
    string nameSource = attr.name();

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

  Attr::Attr(Attr &other)
      : Node(other),
        localName(other.localName),
        name(other.name),
        namespaceURI(other.namespaceURI),
        prefix(other.prefix),
        specified(other.specified),
        value(other.value),
        ownerElement(other.ownerElement),
        attrInternal(other.attrInternal)
  {
  }
}