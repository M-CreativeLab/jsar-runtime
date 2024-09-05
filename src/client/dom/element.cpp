#include <algorithm>
#include "./element.hpp"
#include "./attr.hpp"

namespace dom
{
  Element::Element() : Node()
  {
  }

  Element::Element(pugi::xml_node node) : Node(node)
  {
    auto idAttr = this->internal->attribute("id");
    if (!idAttr.empty())
      id = idAttr.as_string();

    {
      string nameSource = this->internal->name();

      // update the `tagName` to be the uppercase version of the `name`
      tagName.resize(nameSource.size());
      transform(nameSource.begin(), nameSource.end(), tagName.begin(), ::toupper);

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

    {
      /**
       * Parse the `class` attribute.
       */
      auto classAttr = this->internal->attribute("class");
      if (!classAttr.empty())
      {
        className = classAttr.as_string();
        // TODO: implement classList
      }
    }
  }

  Element::Element(Element &other)
      : Node(other),
        id(other.id),
        namespaceURI(other.namespaceURI),
        tagName(other.tagName),
        localName(other.localName),
        className(other.className),
        prefix(other.prefix)
  {
  }

  string Element::getAttribute(const string &name)
  {
    auto attr = this->internal->attribute(name.c_str());
    if (!attr.empty())
      return attr.as_string();
    else
      return "";
  }

  vector<string> Element::getAttributeNames()
  {
    vector<string> names;
    for (auto attr : this->internal->attributes())
      names.push_back(attr.name());
    return names;
  }

  shared_ptr<Attr> Element::getAttributeNode(const string &name)
  {
    auto attr = this->internal->attribute(name.c_str());
    if (!attr.empty())
      return make_shared<Attr>(attr, getPtr<Element>());
    else
      return nullptr;
  }

  bool Element::hasAttribute(const std::string &name)
  {
    return !this->internal->attribute(name.c_str()).empty();
  }

  bool Element::hasAttributes()
  {
    return this->internal->attributes_begin() != this->internal->attributes_end();
  }

  void Element::setId(const string &idValue)
  {
    auto idAttr = this->internal->attribute("id");
    if (idAttr.empty())
      idAttr = this->internal->append_attribute("id");

    if (idAttr.set_value(idValue.c_str()))
      id = idValue;
  }

  void Element::setClassName(const string &classNameValue)
  {
    auto classAttr = this->internal->attribute("class");
    if (classAttr.empty())
      classAttr = this->internal->append_attribute("class");

    if (classAttr.set_value(classNameValue.c_str()))
      className = classNameValue;
  }

  std::string Element::getInnerHTML()
  {
    return "";
  }

  void Element::setInnerHTML(const std::string &html)
  {
  }

  std::string Element::getOuterHTML()
  {
    return "";
  }

  void Element::setOuterHTML(const std::string &html)
  {
  }
}
