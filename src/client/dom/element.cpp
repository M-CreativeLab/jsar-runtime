#include <algorithm>
#include "./element.hpp"
#include "./attr.hpp"

// include the element-specific headers
#include "./html_html_element.hpp"
#include "./html_head_element.hpp"
#include "./html_body_element.hpp"
#include "./html_meta_element.hpp"
#include "./html_script_element.hpp"

namespace dom
{
  shared_ptr<Element> Element::CreateElement(pugi::xml_node node, weak_ptr<Document> ownerDocument)
  {
    string nodeName = node.name();
    if (nodeName == "html")
      return make_shared<HTMLHtmlElement>(node, ownerDocument);
    else if (nodeName == "head")
      return make_shared<HTMLHeadElement>(node, ownerDocument);
    else if (nodeName == "body")
      return make_shared<HTMLBodyElement>(node, ownerDocument);
    else if (nodeName == "meta")
      return make_shared<HTMLMetaElement>(node, ownerDocument);
    else if (nodeName == "script")
      return make_shared<HTMLScriptElement>(node, ownerDocument);
    else
      return make_shared<Element>(node, ownerDocument);
  }

  Element::Element() : Node()
  {
    createdCallback();
  }

  Element::Element(pugi::xml_node node, weak_ptr<Document> ownerDocument) : Node(node, ownerDocument)
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

    createdCallback();
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

  void Element::setAttribute(const string &name, const string &newValue)
  {
    auto attr = this->internal->attribute(name.c_str());
    if (attr.empty())
      attr = this->internal->append_attribute(name.c_str());

    string oldValue = attr.value();
    attr.set_value(newValue.c_str());
    attributeChangedCallback(name, oldValue, newValue);
  }

  void Element::setAttributeNode(shared_ptr<Attr> attr)
  {
    auto attrName = attr->name;
    string oldValue;
    string newValue = attr->value;

    auto attrNode = this->internal->attribute(attrName.c_str());
    if (!attrNode.empty()) // Remmove the existing attribute
    {
      oldValue = attrNode.value();
      this->internal->remove_attribute(attrName.c_str());
    }
    this->internal->append_attribute(attrName.c_str()).set_value(newValue.c_str());
    attributeChangedCallback(attrName, oldValue, newValue);
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
