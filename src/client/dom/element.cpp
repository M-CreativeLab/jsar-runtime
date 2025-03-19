#include <algorithm>
#include <common/utility.hpp>

#include "./element.hpp"
#include "./document.hpp"
#include "./document_fragment.hpp"
#include "./html_element.hpp"
#include "./attr.hpp"
#include "./all_html_elements.hpp"

namespace dom
{
  using namespace std;

  shared_ptr<Element> Element::CreateElement(pugi::xml_node node, shared_ptr<Document> ownerDocument)
  {
    assert(ownerDocument != nullptr && "The owner document is not set when creating an element.");

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

  shared_ptr<Node> Element::CloneElement(shared_ptr<Node> srcNode)
  {
    auto srcElement = dynamic_pointer_cast<Element>(srcNode);
    assert(srcElement != nullptr && "The source node is not an element.");

#define XX(TAGNAME_STR, CLASS_NAME)                                                        \
  if (srcElement->is(TAGNAME_STR))                                                         \
  {                                                                                        \
    shared_ptr<CLASS_NAME> typedSrcElement = dynamic_pointer_cast<CLASS_NAME>(srcElement); \
    assert(typedSrcElement != nullptr && "The source element is not the specific type.");  \
    shared_ptr<Element> clonedElement = make_shared<CLASS_NAME>(*typedSrcElement);         \
    clonedElement->createdCallback();                                                      \
    return clonedElement;                                                                  \
  }
    TYPED_ELEMENT_MAP(XX)
#undef XX

    // Return null if the source element is not a specific type.
    return nullptr;
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

  Element::Element(const Element &other)
      : Node(other),
        id(other.id),
        namespaceURI(other.namespaceURI),
        tagName(other.tagName),
        localName(other.localName),
        prefix(other.prefix),
        classList_(other.classList_),
        attributeNodes_(other.attributeNodes_)
  {
  }

  void Element::createdCallback()
  {
    /**
     * Name && namespace.
     */
    {
      string nameSource = this->nodeName;

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
     * Update the attributes if the internal node is not `nullptr`.
     */
    if (this->internal != nullptr)
    {
      for (auto &item : this->internal->attributes())
      {
        if (item.name() == nullptr) // Skip if the name is `nullptr`
          continue;
        if (item.value() == nullptr) // Reset the value if the value is `nullptr`
          item.set_value("");
        setAttribute(item.name(), item.value());
      }
    }
  }

  void Element::adoptedCallback()
  {
    // TODO: Implement adoptedCallback() for Element
  }

  void Element::attributeChangedCallback(const string &name, const string &oldValue, const string &newValue)
  {
    if (name == "id")
      id = newValue;
    else if (name == "class")
      classList_ = DOMTokenList(newValue, {}, [this](const DOMTokenList &list)
                                { setAttribute("class", list.value(), false /* mute */); });
  }

  void Element::before(std::vector<std::shared_ptr<Node>> nodes)
  {
    auto parent = parentNode.lock();
    if (parent == nullptr || nodes.size() == 0)
      return;

    for (auto &node : nodes)
    {
      if (node == nullptr)
        continue;
      parent->insertBefore(node, getPtr<Element>());
    }
  }

  void Element::before(string text)
  {
    before(getOwnerDocumentChecked().createTextNode(text));
  }

  void Element::after(vector<shared_ptr<Node>> nodes)
  {
    auto parent = parentNode.lock();
    if (parent == nullptr || nodes.size() == 0)
      return;

    auto next = nextSibling();
    for (auto &node : nodes)
    {
      if (node == nullptr)
        continue;

      parent->insertBefore(node, next);
    }
  }

  void Element::after(string text)
  {
    after(getOwnerDocumentChecked().createTextNode(text));
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

  bool Element::hasAttribute(const string &name) const
  {
    return attributeNodes_.find(name) != attributeNodes_.end();
  }

  bool Element::hasAttributes() const
  {
    return !attributeNodes_.empty();
  }

  void Element::setAttribute(const string &name, const string &newValue, bool notify)
  {
    auto attrNode = getAttributeNode(name);
    if (attrNode == nullptr)
      attrNode = Attr::Make(getPtr<Element>(), name, newValue);

    string oldValue = attrNode->value;
    attrNode->value = newValue;
    attributeNodes_[name] = attrNode;

    if (notify)
    {
      // FIXME: attributeChangedCallback() should be called for creating a new attribute?
      attributeChangedCallback(name, oldValue, newValue);
    }
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

  void Element::remove()
  {
    if (parentNode.expired())
      return;

    auto parent = parentNode.lock();
    if (parent == nullptr)
      return;

    parent->removeChild(getPtr<Element>());
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
    setAttribute("id", idValue);
  }

  string Element::getInnerHTML()
  {
    return dom::SerializeFragment(getPtr<Element>(), false);
  }

  void Element::setInnerHTML(const string &markup)
  {
    auto self = getPtr<Element>();
    auto fragment = dom::ParseFragment(self, markup);
    if (fragment != nullptr)
    {
      if (Node::Is<HTMLTemplateElement>(self))
      {
        auto &templateElement = Node::AsChecked<HTMLTemplateElement>(self);
        auto contents = templateElement.getContent();
        contents->replaceAll(fragment);
      }
      else
      {
        replaceAll(fragment);
      }
    }
  }

  string Element::getOuterHTML()
  {
    return "";
  }

  void Element::setOuterHTML(const string &html)
  {
  }
}
