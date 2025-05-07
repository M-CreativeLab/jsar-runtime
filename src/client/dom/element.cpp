#include <algorithm>
#include <common/utility.hpp>
#include <client/browser/window.hpp>
#include <client/builtin_scene/ecs-inl.hpp>
#include <client/html/html_element.hpp>
#include <client/html/all_html_elements.hpp>

#include "./events/mouse_event.hpp"
#include "./events/pointer_event.hpp"
#include "./element.hpp"
#include "./document.hpp"
#include "./document_fragment.hpp"
#include "./attr.hpp"

namespace dom
{
  using namespace std;
  using namespace builtin_scene;

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

    // Just clone as `HTMLElement` if the source element is not supported yet.
    shared_ptr<HTMLElement> clonedElement;
    {
      auto typedSrcElement = dynamic_pointer_cast<HTMLElement>(srcElement);
      clonedElement = make_shared<HTMLElement>(*typedSrcElement);
      clonedElement->createdCallback();
    }
    return dynamic_pointer_cast<Element>(clonedElement);
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

  std::shared_ptr<Element> Element::getAnimationTarget()
  {
    return getPtr<Element>();
  }

  void Element::connectedCallback()
  {
    Node::connectedCallback();

    initCSSBoxes();
    adoptStyleDirectly(defaultStyle_);
  }

  void Element::disconnectedCallback()
  {
    resetCSSBoxes();
    Node::disconnectedCallback();
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

    /**
     * Update the scene object.
     */
    auto ownerDocument = getOwnerDocumentReferenceAs<HTMLDocument>(false);
    if (ownerDocument != nullptr)
      scene_ = ownerDocument->scene;
  }

  void Element::adoptedCallback()
  {
    // TODO: Implement adoptedCallback() for Element
  }

  void Element::attributeChangedCallback(const string &name, const string &oldValue, const string &newValue)
  {
    markAsDirty();

    if (name == "id")
    {
      id = newValue;
      return;
    }

    if (name == "class")
    {
      auto onClassListChanged = [this](const DOMTokenList &list)
      {
        setAttribute("class", list.value(), false /* mute */);
        classListChangedCallback(list);
      };
      classList_ = DOMTokenList(newValue, {}, onClassListChanged);
    }
  }

  void Element::classListChangedCallback(const DOMTokenList &newClassList)
  {
    markAsDirty();
  }

  void Element::actionStateChangedCallback()
  {
    markAsDirty();
  }

  void Element::styleAdoptedCallback()
  {
    auto ownerDocument = getOwnerDocumentReferenceAs<HTMLDocument>(true);
    if (ownerDocument != nullptr && adoptedStyle_->hasProperty("display"))
    {
      auto newDisplay = adoptedStyle_->getPropertyValue("display");
      if (newDisplay != currentDisplayStr_)
      {
        currentDisplayStr_ = newDisplay;
        reinitCSSBoxes();
      }
    }
  }

  void Element::initCSSBoxes()
  {
    auto ownerDocument = getOwnerDocumentReferenceAs<HTMLDocument>(false);
    if (ownerDocument != nullptr && isRenderable())
    {
      resetCSSBoxes(true); // Clear the existing boxes.

      auto &layoutView = ownerDocument->layoutViewRef();
      shared_ptr<client_layout::LayoutBlock> parentBlock = nullptr;
      {
        auto parentElement = getParentElement();
        if (parentElement != nullptr)
          parentBlock = dynamic_pointer_cast<client_layout::LayoutBlock>(parentElement->principalBox_);
      }
      principalBox_ = layoutView.createBox(currentDisplayStr_, getPtr<Element>(), parentBlock);
      boxes_ = {principalBox_};
    }
  }

  void Element::reinitCSSBoxes()
  {
    assert(principalBox_ != nullptr &&
           "The principal box should not be null when reinitializing CSS boxes.");
    assert(isRenderable() &&
           "The element should be renderable when reinitializing CSS boxes.");

    auto ownerDocument = getOwnerDocumentReferenceAs<HTMLDocument>(false);
    assert(ownerDocument != nullptr && isRenderable() &&
           "The owner document is not set when reinitializing CSS boxes.");
    {
      auto &layoutView = ownerDocument->layoutViewRef();
      shared_ptr<client_layout::LayoutBlock> parentBlock = nullptr;
      auto parentElement = getParentElement();
      if (parentElement != nullptr)
        parentBlock = dynamic_pointer_cast<client_layout::LayoutBlock>(parentElement->principalBox_);

      auto newPrincipalBox = layoutView.createBox(currentDisplayStr_, getPtr<Element>(),
                                                  parentBlock,
                                                  principalBox_->nextSibling());

      layoutView.removeObject(principalBox_); // Remove the old box.
      principalBox_ = newPrincipalBox;
      boxes_ = {principalBox_};
    }
    assert(principalBox_ != nullptr &&
           "The principal box is not set when reinitializing CSS boxes.");

    // Skip the following steps to create child boxes if the principal box is a none box.
    if (principalBox_->isNone())
      return;

    // Recursively initialize the CSS boxes of the child nodes.
    function<void(shared_ptr<Node>)> initBox = [&initBox](shared_ptr<Node> node)
    {
      if (Node::Is<Text>(node))
      {
        auto &textNode = Node::AsChecked<Text>(node);
        textNode.resetCSSBoxes();
        textNode.initCSSBoxes();
        return;
      }
      if (Node::Is<Element>(node))
      {
        Node::AsChecked<Element>(node).initCSSBoxes();
        for (auto childNode : node->childNodes)
          initBox(childNode);
        return;
      }

      // Skip if the node is not an element or text node.
      // FIXME(yorkie): do nothing when the node is not an element or text node, such as comment node.
    };

    // Iterate the child nodes recursively and initialize the CSS boxes.
    for (auto childNode : childNodes)
      initBox(childNode);
  }

  void Element::resetCSSBoxes(bool skipCheck)
  {
    if (principalBox_ == nullptr || boxes_.empty())
      return;

    shared_ptr<HTMLDocument> ownerDocument = getOwnerDocumentReferenceAs<HTMLDocument>(false);
    if (!skipCheck &&
        (TR_UNLIKELY(ownerDocument == nullptr) || !isRenderable()))
      return;

    assert(ownerDocument != nullptr && "The owner document is not set when resetting CSS boxes.");
    auto &layoutView = ownerDocument->layoutViewRef();
    for (auto &box : boxes_)
      layoutView.removeObject(box);
    principalBox_.reset();
    boxes_.clear();
  }

  bool Element::adoptStyle(const client_cssom::CSSStyleDeclaration &style)
  {
    client_cssom::CSSStyleDeclaration newStyle = style;
    newStyle.update(defaultStyle_, true); // Update the default style if these properties are not present.

    if (adoptedStyle_ != nullptr &&      // Pass if `adoptedStyle_` is not set.
        adoptedStyle_->equals(newStyle)) // Skip if the style is the same.
      return false;

    return adoptStyleDirectly(newStyle);
  }

  void Element::useSceneWithCallback(const function<void(builtin_scene::Scene &)> &callback)
  {
    auto sceneRef = scene_.lock();
    if (sceneRef != nullptr)
      callback(*sceneRef);
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

  geometry::DOMRect Element::getBoundingClientRect() const
  {
    auto clientRects = getClientRects();
    if (clientRects.empty())
      return dom::geometry::DOMRect();
    if (clientRects.size() == 1)
      return clientRects[0];

    // Merge the client rects into a single bounding box.
    dom::geometry::DOMRect boundingRect(0, 0, 0, 0);
    for (auto &rect : clientRects)
    {
      boundingRect.x() = std::min(boundingRect.x(), rect.x());
      boundingRect.y() = std::min(boundingRect.y(), rect.y());
      boundingRect.width() = std::max(boundingRect.width(), rect.width());
      boundingRect.height() = std::max(boundingRect.height(), rect.height());
    }
    return boundingRect;
  }

  vector<geometry::DOMRect> Element::getClientRects() const
  {
    vector<geometry::DOMRect> clientRects = getLayoutRects();
    if (clientRects.empty())
      return clientRects;

    shared_ptr<browser::Window> window = getOwnerDocumentChecked().defaultView();
    // glm::vec2 accumulatedScrollOffset(window->scrollX(), window->scrollY());
    // auto ancestors = getAncestors(false);
    // for (auto ancestor : ancestors)
    // {
    //   shared_ptr<Node> node = const_pointer_cast<Node>(ancestor);
    //   if (Node::Is<const Element>(node))
    //   {
    //     const auto &element = Node::AsChecked<const Element>(node);
    //     accumulatedScrollOffset.x += element.scrollLeft;
    //     accumulatedScrollOffset.y += element.scrollTop;
    //   }
    // }

    // for (auto &rect : clientRects)
    // {
    //   rect.x() += accumulatedScrollOffset.x;
    //   rect.y() += accumulatedScrollOffset.y;
    // }
    return clientRects;
  }

  bool Element::checkVisibility(CheckVisibilityOptions options) const
  {
    // TODO: Implement checkVisibility() for Element
    return true;
  }

  void Element::scrollTo(const ScrollOptions &options)
  {
    auto layoutBox = principalBox();
    if (layoutBox == nullptr || !layoutBox->isBox())
      return;

    glm::vec3 offset = glm::vec3(options.left, options.top, 0);
    dynamic_pointer_cast<client_layout::LayoutBox>(layoutBox)->scrollTo(offset);
    dispatchEvent(make_shared<dom::Event>(DOMEventConstructorType::kEvent, DOMEventType::Scroll));

    // TODO(yorkie): dispatching this event when the scroll is finished.
    dispatchEvent(make_shared<dom::Event>(DOMEventConstructorType::kEvent, DOMEventType::ScrollEnd));
  }

  void Element::scrollBy(const ScrollOptions &options)
  {
    auto layoutBox = principalBox();
    if (layoutBox == nullptr || !layoutBox->isBox())
      return;

    glm::vec3 offset = glm::vec3(options.left, options.top, 0);
    dynamic_pointer_cast<client_layout::LayoutBox>(layoutBox)->scrollBy(offset);
    dispatchEvent(make_shared<dom::Event>(DOMEventConstructorType::kEvent, DOMEventType::Scroll));

    // TODO(yorkie): dispatching this event when the scroll is finished.
    dispatchEvent(make_shared<dom::Event>(DOMEventConstructorType::kEvent, DOMEventType::ScrollEnd));
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
        auto &templateElement = Node::AsChecked<HTMLTemplateElement>(shared_from_this());
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
    // TODO: Implement getOuterHTML() for Element
    return "";
  }

  void Element::setOuterHTML(const string &html)
  {
    // TODO: Implement setOuterHTML() for Element
  }

  void Element::dispatchEventInternal(std::shared_ptr<dom::Event> event)
  {
    dispatchEvent(event);

    // If the event bubbles, dispatch it to the parent element.
    if (event->bubbles())
    {
      auto parentElement = getParentElement();
      if (parentElement != nullptr)
        parentElement->dispatchEventInternal(event);
    }
  }

  void Element::simulateMouseDown(const glm::vec3 &hitPointInWorld)
  {
    dispatchEventInternal(events::MouseEvent::MouseDown());
    dispatchEventInternal(events::PointerEvent::PointerDown());
  }

  void Element::simulateMouseUp(const glm::vec3 &hitPointInWorld)
  {
    dispatchEventInternal(events::MouseEvent::MouseUp());
    dispatchEventInternal(events::PointerEvent::PointerUp());
  }

  void Element::simulateMouseMove(const glm::vec3 &hitPointInWorld)
  {
    dispatchEventInternal(events::MouseEvent::MouseMove());
    dispatchEventInternal(events::PointerEvent::PointerMove());
  }

  void Element::simulateMouseOut(const glm::vec3 &hitPointInWorld)
  {
    dispatchEventInternal(events::MouseEvent::MouseOut());
    dispatchEventInternal(events::PointerEvent::PointerOut());
  }

  void Element::simulateMouseOver(const glm::vec3 &hitPointInWorld)
  {
    setActionState(is_hovered_, true);
    dispatchEventInternal(events::MouseEvent::MouseOver());
    dispatchEventInternal(events::PointerEvent::PointerOver());
  }

  void Element::simulateMouseEnter(const glm::vec3 &hitPointInWorld)
  {
    if (is_hovered_)
      return;
    setActionState(is_hovered_, true);

    dispatchEventInternal(events::MouseEvent::MouseEnter());
    dispatchEventInternal(events::PointerEvent::PointerEnter());
  }

  void Element::simulateMouseLeave(const glm::vec3 &hitPointInWorld)
  {
    if (!is_hovered_)
      return;
    setActionState(is_hovered_, false);

    dispatchEventInternal(events::MouseEvent::MouseLeave());
    dispatchEventInternal(events::PointerEvent::PointerLeave());
  }

  void Element::simulateClick(const glm::vec3 &hitPointInWorld)
  {
    dispatchEventInternal(events::PointerEvent::Click());
  }

  void Element::simulateScrollWithOffset(float offsetX, float offsetY)
  {
    auto layoutBox = dynamic_pointer_cast<client_layout::LayoutBox>(principalBox());
    if (layoutBox == nullptr)
      return;
    assert(layoutBox->isBox() && "The layout box is not a box.");

    glm::vec3 offset;
    if (layoutBox->scrollsOverflowX())
      offset.x = offsetX;
    if (layoutBox->scrollsOverflowY())
      offset.y = offsetY;

    if (offset.x == 0 && offset.y == 0)
      return;

    layoutBox->scrollBy(offset);
    dispatchEvent(make_shared<dom::Event>(DOMEventConstructorType::kEvent, DOMEventType::Scroll));
  }

  bool Element::setActionState(bool &state, bool value)
  {
    if (state != value)
    {
      state = value;
      actionStateChangedCallback();
      return true;
    }
    else
    {
      return false;
    }
  }

  bool Element::adoptStyleDirectly(const client_cssom::CSSStyleDeclaration &newStyle)
  {
    adoptedStyle_ = make_unique<client_cssom::CSSStyleDeclaration>(newStyle);
    styleAdoptedCallback();

    // Update the layout node style.
    bool updated = false;
    for (auto box : boxes_)
    {
      if (box->setStyle(*adoptedStyle_))
        updated = true;
    }
    return updated;
  }

  std::shared_ptr<Element> Element::firstElementChild() const
  {
    for (auto childNode : childNodes)
    {
      if (childNode->nodeType == NodeType::ELEMENT_NODE)
        return Node::As<Element>(childNode);
    }
    return nullptr;
  }

  std::shared_ptr<Element> Element::lastElementChild() const
  {
    for (auto it = childNodes.rbegin(); it != childNodes.rend(); it++)
    {
      shared_ptr<Node> childNode = *it;
      if (childNode->nodeType == NodeType::ELEMENT_NODE)
        return Node::As<Element>(childNode);
    }
    return nullptr;
  }
}
