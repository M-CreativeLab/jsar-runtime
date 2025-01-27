#include <algorithm>
#include <common/utility.hpp>

#include "./element.hpp"
#include "./html_element.hpp"
#include "./attr.hpp"
#include "./all_html_elements.hpp"

namespace dom
{
  using namespace std;

  shared_ptr<Element> Element::CreateElement(pugi::xml_node node, shared_ptr<Document> ownerDocument)
  {
    string nodeName = node.name();
#define XX(tagName, className)                                                 \
  if (nodeName == tagName)                                                     \
  {                                                                            \
    shared_ptr<Element> element = make_shared<className>(node, ownerDocument); \
    element->createdCallback();                                                \
    return element;                                                            \
  }
    TYPED_ELEMENT_MAP(XX)
#undef XX

    shared_ptr<HTMLElement> element = make_shared<HTMLElement>(node, ownerDocument);
    element->createdCallback();
    return dynamic_pointer_cast<Element>(element);
  }

  shared_ptr<Element> Element::CreateElement(string namespaceURI, string tagName, shared_ptr<Document> ownerDocument)
  {
#define XX(tagNameStr, className)                                                 \
  if (tagName == tagNameStr)                                                      \
  {                                                                               \
    shared_ptr<Element> element = make_shared<className>(tagName, ownerDocument); \
    element->namespaceURI = namespaceURI;                                         \
    element->createdCallback();                                                   \
    return element;                                                               \
  }
    TYPED_ELEMENT_MAP(XX)
#undef XX

    shared_ptr<HTMLElement> element = make_shared<HTMLElement>(tagName, ownerDocument);
    element->namespaceURI = namespaceURI;
    element->createdCallback();
    return dynamic_pointer_cast<Element>(element);
  }

  Element::Element(string tagName, optional<shared_ptr<Document>> ownerDocument)
      : Node(NodeType::ELEMENT_NODE, tagName, ownerDocument),
        tagName(ToUpperCase(tagName))
  {
  }

  Element::Element(pugi::xml_node node, shared_ptr<Document> ownerDocument)
      : Node(node, ownerDocument)
  {
  }

  Element::Element(Element &other)
      : Node(other),
        id(other.id),
        namespaceURI(other.namespaceURI),
        tagName(other.tagName),
        localName(other.localName),
        prefix(other.prefix),
        classList_(other.classList_)
  {
  }

  void Element::createdCallback()
  {
    /**
     * Name && namespace.
     */
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

    /**
     * Attributes.
     */
    for (auto &item : this->internal->attributes())
      attributeNodes_[item.name()] = Attr::Make(getPtr<Element>(), item);

    /**
     * `id` attribute.
     */
    auto idAttr = getAttributeNode("id");
    if (idAttr != nullptr)
      id = idAttr->value;

    {
      /**
       * `class` attribute.
       */
      auto classAttr = getAttributeNode("class");
      if (classAttr != nullptr)
        classList_ = DOMTokenList(classAttr->value, {}, [](const DOMTokenList &list)
                                  {
                                    // update the `class` attribute
                                    // TODO: update the attribute value
                                  });
    }
  }

  string Element::getAttribute(const string &name) const
  {
    auto it = attributeNodes_.find(name);
    if (it != attributeNodes_.end())
      return it->second->value;
    else
      return "";
  }

  vector<string> Element::getAttributeNames() const
  {
    vector<string> names;
    for (auto &attrNode : attributeNodes_)
      names.push_back(attrNode.first);
    return names;
  }

  shared_ptr<Attr> Element::getAttributeNode(const string &name) const
  {
    auto it = attributeNodes_.find(name);
    if (it != attributeNodes_.end())
      return it->second;
    else
      return nullptr;
  }

  Attr &Element::getAttributeNodeChecked(const string &name) const
  {
    auto it = attributeNodes_.find(name);
    assert(it != attributeNodes_.end() && "The attribute node is not found.");
    return *it->second;
  }

  bool Element::hasAttribute(const std::string &name) const
  {
    return attributeNodes_.find(name) != attributeNodes_.end();
  }

  bool Element::hasAttributes() const
  {
    return !attributeNodes_.empty();
  }

  void Element::setAttribute(const string &name, const string &newValue)
  {
    auto attrNode = getAttributeNode(name);
    if (attrNode == nullptr)
      attrNode = Attr::Make(getPtr<Element>(), name, newValue);

    string oldValue = attrNode->value;
    attrNode->value = newValue;
    // FIXME: attributeChangedCallback() should be called for creating a new attribute?
    attributeChangedCallback(name, oldValue, newValue);
  }

  void Element::setAttributeNode(shared_ptr<Attr> attr)
  {
    auto attrName = attr->name;
    string oldValue;
    string newValue = attr->value;

    auto attrNode = getAttributeNode(attrName);
    if (attrNode != nullptr) // Remmove the existing attribute
    {
      oldValue = attrNode->value;
      attributeNodes_.erase(attrName);
    }
    attributeNodes_[attrName] = attr;
    attributeChangedCallback(attrName, oldValue, newValue);
  }

  void Element::removeAttribute(const string &name)
  {
    auto attr = getAttributeNode(name);
    if (attr != nullptr)
    {
      string oldValue = attr->value;
      attributeNodes_.erase(name);
      attributeChangedCallback(name, oldValue, "");
    }
  }

  bool Element::is(const string expectedTagName)
  {
    string expectedTagNameUpper;
    expectedTagNameUpper.resize(expectedTagName.size());
    transform(expectedTagName.begin(), expectedTagName.end(), expectedTagNameUpper.begin(), ::toupper);
    return tagName == expectedTagNameUpper;
  }

  void Element::setId(const string &idValue)
  {
    auto idAttr = this->internal->attribute("id");
    if (idAttr.empty())
      idAttr = this->internal->append_attribute("id");

    if (idAttr.set_value(idValue.c_str()))
      id = idValue;
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
