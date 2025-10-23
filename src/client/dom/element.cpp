#include <algorithm>
#include <common/utility.hpp>
#include <client/browser/window.hpp>
#include <client/builtin_scene/ecs-inl.hpp>
#include <client/cssom/values/computed/context.hpp>
#include <client/html/html_element.hpp>
#include <client/html/all_html_elements.hpp>
#include <client/script_bindings/events/all_events.hpp>

#include "./events/mouse_event.hpp"
#include "./events/pointer_event.hpp"
#include "./browsing_context.hpp"
#include "./element.hpp"
#include "./document.hpp"
#include "./document_fragment.hpp"
#include "./attr.hpp"

namespace dom
{
  using namespace std;
  using namespace builtin_scene;
  using namespace client_cssom;

  shared_ptr<Element> Element::CreateElement(pugi::xml_node node, shared_ptr<Document> ownerDocument)
  {
    assert(ownerDocument != nullptr && "The owner document is not set when creating an element.");

    string nodeName = node.name();
    shared_ptr<Element> newElement = nullptr;

    try
    {
#define XX(tagName, className)                                \
  if (nodeName == tagName)                                    \
  {                                                           \
    newElement = make_shared<className>(node, ownerDocument); \
  }
      TYPED_ELEMENT_MAP(XX)
#undef XX
    }
    catch (const exception &e)
    {
      cerr << "Failed to create element from node '" << nodeName << "': " << e.what() << endl;
    }

    if (newElement == nullptr) [[unlikely]]
    {
      newElement = make_shared<HTMLElement>(node, ownerDocument);
      assert(newElement != nullptr &&
             "Failed to create an element from the node.");
    }
    newElement->createdCallback(false);
    return newElement;
  }

  shared_ptr<Element> Element::CreateElement(string namespaceURI, string tagName, shared_ptr<Document> ownerDocument, bool from_scripting)
  {
    shared_ptr<Element> newElement = nullptr;
    try
    {
#define XX(tagNameStr, className)                                \
  if (tagName == tagNameStr)                                     \
  {                                                              \
    newElement = make_shared<className>(tagName, ownerDocument); \
  }
      TYPED_ELEMENT_MAP(XX)
#undef XX
    }
    catch (const exception &e)
    {
      cerr << "Failed to create element from name '"
           << namespaceURI << "." << tagName << "': " << e.what() << endl;
    }

    if (newElement == nullptr) [[unlikely]]
    {
      newElement = make_shared<HTMLElement>(tagName, ownerDocument);
      assert(newElement != nullptr &&
             "Failed to create an element from the tag name.");
    }
    newElement->namespaceURI = namespaceURI;
    newElement->createdCallback(from_scripting);
    return newElement;
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
    clonedElement->createdCallback(true);                                                  \
    return clonedElement;                                                                  \
  }
    TYPED_ELEMENT_MAP(XX)
#undef XX

    // Just clone as `HTMLElement` if the source element is not supported yet.
    shared_ptr<HTMLElement> clonedElement;
    {
      auto typedSrcElement = dynamic_pointer_cast<HTMLElement>(srcElement);
      clonedElement = make_shared<HTMLElement>(*typedSrcElement);
      clonedElement->createdCallback(true);
    }
    return dynamic_pointer_cast<Element>(clonedElement);
  }

  Element::Element(string tagName, optional<shared_ptr<Document>> ownerDocument)
      : Node(NodeType::ELEMENT_NODE, tagName, ownerDocument)
      , tagName(ToUpperCase(tagName))
  {
  }

  Element::Element(pugi::xml_node node, shared_ptr<Document> ownerDocument)
      : Node(node, ownerDocument)
  {
  }

  Element::Element(const Element &other)
      : Node(other)
      , id(other.id)
      , namespaceURI(other.namespaceURI)
      , tagName(other.tagName)
      , localName(other.localName)
      , prefix(other.prefix)
      , classList_(other.classList_)
      , attributeNodes_(other.attributeNodes_)
  {
  }

  vector<geometry::DOMRect> Element::getLayoutRects() const
  {
    vector<geometry::DOMRect> rects;
    for (const auto &box : boxes_)
    {
      if (box == nullptr)
        continue; // Skip null boxes.

      // Get the bounding box of the layout box.
      auto fragment = box->fragment();
      geometry::DOMRect rect(fragment.left(), fragment.top(), fragment.width(), fragment.height());
      rects.push_back(rect);
    }
    return rects;
  }

  std::shared_ptr<Element> Element::getAnimationTarget()
  {
    return getPtr<Element>();
  }

  void Element::connectedCallback()
  {
    Node::connectedCallback();

    initCSSBoxes();
    {
      auto window = getOwnerDocumentReferenceAs<HTMLDocument>(true)->defaultView();
      assert(window != nullptr &&
             "The window must not be null in a TextNode().");
      auto initial_style = window->createComputedStyle(shared_from_this(), nullopt, false);
      recalcStyleDirectly(*initial_style);
    }
  }

  void Element::disconnectedCallback()
  {
    resetCSSBoxes();
    Node::disconnectedCallback();
  }

  void Element::createdCallback(bool from_scripting)
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

    /**
     * Initialize the element's animations.
     */
    element_animations_ = make_shared<ElementAnimations>(getPtr<Element>());
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
    if (ownerDocument != nullptr && adopted_style_->hasProperty("display"))
    {
      auto newDisplay = adopted_style_->getPropertyValue("display");
      if (newDisplay != currentDisplayStr_)
      {
        currentDisplayStr_ = newDisplay;
        reinitCSSBoxes();
      }
    }
  }

  void Element::layoutSizeChangedCallback(const client_layout::Fragment &)
  {
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
      if (principalBox_ != nullptr)
      {
        boxes_ = {principalBox_};
      }
      else
      {
        boxes_.clear();
      }
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

      auto newPrincipalBox = layoutView.createBox(currentDisplayStr_,
                                                  getPtr<Element>(),
                                                  parentBlock,
                                                  principalBox_->nextSibling());

      layoutView.removeObject(principalBox_); // Remove the old box.
      principalBox_ = newPrincipalBox;
      if (principalBox_ != nullptr)
      {
        boxes_ = {principalBox_};
      }
      else
      {
        // Parent is a replaced element, don't create layout box for this element
        boxes_.clear();
      }
    }
    // Only assert if we expect a principal box (when parent is not a replaced element)
    if (principalBox_ == nullptr)
    {
      // Element is a child of a replaced element, skip further initialization
      return;
    }

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

  bool Element::adoptStyle(const ComputedStyle &new_style)
  {
    if (adopted_style_ != nullptr && // Pass if `adopted_style_` is not set.
        ComputedStyle::ComputeDifference(new_style, *adopted_style_) == ComputedStyle::kEqual)
      return false;
    return recalcStyleDirectly(new_style);
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

  string Element::getAttribute(const string &name, const std::string &defaultValue) const
  {
    auto it = attributeNodes_.find(name);
    if (it != attributeNodes_.end())
      return it->second->value;
    else
      return defaultValue;
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
      return {};

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

    // Throttle scroll events for better performance
    if (!shouldThrottleScrollEvent())
    {
      last_scroll_event_time_ = std::chrono::steady_clock::now();
      dispatchEvent(make_shared<dom::Event>(DOMEventConstructorType::kEvent, DOMEventType::Scroll));
    }

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

    // Throttle scroll events for better performance
    if (!shouldThrottleScrollEvent())
    {
      last_scroll_event_time_ = std::chrono::steady_clock::now();
      dispatchEvent(make_shared<dom::Event>(DOMEventConstructorType::kEvent, DOMEventType::Scroll));
    }

    // TODO(yorkie): dispatching this event when the scroll is finished.
    dispatchEvent(make_shared<dom::Event>(DOMEventConstructorType::kEvent, DOMEventType::ScrollEnd));
  }

  RenderQueue Element::computeRenderQueue() const
  {
    auto renderQueue = Node::computeRenderQueue();

    // Update the translateZ from the layout box
    auto layoutBox = dynamic_pointer_cast<const client_layout::LayoutBox>(principalBox());
    if (layoutBox != nullptr) [[likely]]
      renderQueue.translateZ = layoutBox->getTranslateZ();

    // Update the zIndex from the adopted style
    if (adopted_style_ != nullptr) [[likely]]
    {
      RenderQueue containerRenderQueue;
      auto containerBox = principalBox()->containerForAbsolutePosition();
      if (containerBox != nullptr)
        containerRenderQueue = containerBox->nodeRef().getRenderQueue();

      bool isThisPositioned = adopted_style_->isPositioned();
      if (isThisPositioned)
      {
        if (!adopted_style_->hasZIndex())
        {
          renderQueue.zIndex = containerRenderQueue.zIndex + 0.1f;
        }
        else
        {
          renderQueue.zIndex = adopted_style_->zIndex().value_or(0);
        }
      }
      else
      {
        renderQueue.zIndex = containerRenderQueue.zIndex;
      }
    }

    return renderQueue;
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
    auto mouseEvent = events::MouseEvent::MouseDown();
    mouseEvent->setTarget(shared_from_this());
    dispatchEventInternal(std::move(mouseEvent));

    auto pointerEvent = events::PointerEvent::PointerDown();
    pointerEvent->setTarget(shared_from_this());
    dispatchEventInternal(std::move(pointerEvent));
  }

  void Element::simulateMouseUp(const glm::vec3 &hitPointInWorld)
  {
    auto mouseEvent = events::MouseEvent::MouseUp();
    mouseEvent->setTarget(shared_from_this());
    dispatchEventInternal(std::move(mouseEvent));

    auto pointerEvent = events::PointerEvent::PointerUp();
    pointerEvent->setTarget(shared_from_this());
    dispatchEventInternal(std::move(pointerEvent));
  }

  void Element::simulateMouseMove(const glm::vec3 &hitPointInWorld)
  {
    auto mouseEvent = events::MouseEvent::MouseMove();
    mouseEvent->setTarget(shared_from_this());
    dispatchEventInternal(std::move(mouseEvent));

    auto pointerEvent = events::PointerEvent::PointerMove();
    pointerEvent->setTarget(shared_from_this());
    dispatchEventInternal(std::move(pointerEvent));
  }

  void Element::simulateMouseOut(const glm::vec3 &hitPointInWorld)
  {
    auto mouseEvent = events::MouseEvent::MouseOut();
    mouseEvent->setTarget(shared_from_this());
    dispatchEventInternal(std::move(mouseEvent));

    auto pointerEvent = events::PointerEvent::PointerOut();
    pointerEvent->setTarget(shared_from_this());
    dispatchEventInternal(std::move(pointerEvent));
  }

  void Element::simulateMouseOver(const glm::vec3 &hitPointInWorld)
  {
    auto mouseEvent = events::MouseEvent::MouseOver();
    mouseEvent->setTarget(shared_from_this());
    dispatchEventInternal(std::move(mouseEvent));

    auto pointerEvent = events::PointerEvent::PointerOver();
    pointerEvent->setTarget(shared_from_this());
    dispatchEventInternal(std::move(pointerEvent));
  }

  void Element::simulateMouseEnter(const glm::vec3 &hitPointInWorld)
  {
    if (is_hovered_)
      return;
    setActionState(is_hovered_, true);

    auto mouseEvent = events::MouseEvent::MouseEnter();
    mouseEvent->setTarget(shared_from_this());
    dispatchEventInternal(std::move(mouseEvent));

    auto pointerEvent = events::PointerEvent::PointerEnter();
    pointerEvent->setTarget(shared_from_this());
    dispatchEventInternal(std::move(pointerEvent));
  }

  void Element::simulateMouseLeave(const glm::vec3 &hitPointInWorld)
  {
    if (!is_hovered_)
      return;
    setActionState(is_hovered_, false);

    auto mouseEvent = events::MouseEvent::MouseLeave();
    mouseEvent->setTarget(shared_from_this());
    dispatchEventInternal(std::move(mouseEvent));

    auto pointerEvent = events::PointerEvent::PointerLeave();
    pointerEvent->setTarget(shared_from_this());
    dispatchEventInternal(std::move(pointerEvent));
  }

  void Element::simulateClick(const glm::vec3 &hitPointInWorld)
  {
    // Create a click event object to pass to the handler
    auto clickEvent = events::PointerEvent::Click();
    clickEvent->setTarget(shared_from_this());

    // Execute click handler if it exists, passing the event object
    if (hasEventHandler("click"))
      executeEventHandler("click", clickEvent.get());

    dispatchEventInternal(std::move(clickEvent));
  }

  bool Element::simulateScrollWithOffset(float offsetX, float offsetY)
  {
    auto layoutBox = dynamic_pointer_cast<client_layout::LayoutBox>(principalBox());
    if (layoutBox == nullptr ||
        !layoutBox->isScrollContainer())
      return false;
    assert(layoutBox->isBox() && "The layout box is not a box.");

    glm::vec3 offset = glm::vec3(0, 0, 0);
    if (layoutBox->scrollsOverflowX())
      offset.x = offsetX;
    if (layoutBox->scrollsOverflowY())
      offset.y = offsetY;
    // TODO(yorkie): support z-axis scrolling

    if (offset.x == 0 && offset.y == 0)
      return false;

    bool scrolled = layoutBox->scrollBy(offset);
    if (scrolled)
    {
      markAsDirty();

      // Throttle scroll events for better performance
      if (!shouldThrottleScrollEvent())
      {
        last_scroll_event_time_ = chrono::steady_clock::now();
        auto scrollEvent = make_shared<dom::Event>(DOMEventConstructorType::kEvent, DOMEventType::Scroll);
        scrollEvent->setTarget(shared_from_this());
        dispatchEvent(scrollEvent);
      }
    }
    return scrolled;
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

  bool Element::shouldThrottleScrollEvent() const
  {
    auto now = chrono::steady_clock::now();
    return (now - last_scroll_event_time_) < scroll_throttle_duration_;
  }

  bool Element::recalcStyleDirectly(const client_cssom::ComputedStyle &new_computed_style)
  {
    ElementAnimations &element_animations = elementAnimationsRef();
    CSSAnimations &css_animations = element_animations.cssAnimations();

    // 1. Update the css transitions if the new computed style has transition properties.
    if (new_computed_style.hasTransitionProperties())
    {
      auto owner_document = getOwnerDocumentReferenceAs<HTMLDocument>(true);
      css_animations.setTransitions(new_computed_style, owner_document->timeline());
    }
    else
    {
      css_animations.clearTransitions();
    }

    // 2. Update the adopted style.
    adopted_style_ = make_unique<client_cssom::ComputedStyle>(new_computed_style);
    styleAdoptedCallback();

    // 3. Compute the animated style.
    client_cssom::ComputedStyle animated_style = *adopted_style_;
    if (!element_animations.isEmpty())
    {
      bool is_animated = element_animations.updateFrameToStyle(animated_style);
      if (is_animated)
      {
        // If the element has animations to apply, mark the element as dirty thus it could be re-calculated in the next
        // frame.
        markAsDirty();
      }
    }

    // 4. Update the layout node style from the animated style.
    bool updated = false;
    for (auto box : boxes_)
    {
      if (box->setStyle(animated_style))
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

  // Generalized event handler methods

  void Element::setEventHandler(const string &type, const string &handlerCode)
  {
    if (type.empty() || handlerCode.empty())
      return;
    event_handler_codes_[type] = handlerCode;
  }

  bool Element::hasEventHandler(const string &type) const
  {
    // Check if we have handler code for this type
    auto it = event_handler_codes_.find(type);
    return it != event_handler_codes_.end() && !it->second.empty();
  }

  string Element::getEventHandlerCode(const string &type) const
  {
    auto it = event_handler_codes_.find(type);
    if (it != event_handler_codes_.end())
      return it->second;

    return "";
  }

  void Element::executeEventHandler(const string &type, Event *event)
  {
    // Skip if no event is provided and no handler code exists
    if (event == nullptr) [[unlikely]]
      return;

    // Check for handler code
    string handlerCode = getEventHandlerCode(type);
    if (handlerCode.empty())
      return;

    // Get the document and its browsing context to execute the JavaScript
    auto ownerDoc = getOwnerDocumentReference();
    if (ownerDoc == nullptr)
      return;

    auto browsingContext = ownerDoc->browsingContext;
    if (browsingContext == nullptr || browsingContext->scriptingContext == nullptr)
      return;

    v8::Isolate *isolate = browsingContext->scriptingContext->isolate();
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);

    try
    {
      // Use the new compileEventHandler method to compile the handler as a function
      auto maybeFunction = browsingContext->scriptingContext->compileFunction(handlerCode);

      v8::Local<v8::Function> handlerFunction;
      if (!maybeFunction.ToLocal(&handlerFunction))
      {
        cerr << "Failed to compile " << type << " handler: " << handlerCode << endl;
        return;
      }

      // Get the V8 context for function execution
      v8::Isolate *isolate = browsingContext->scriptingContext->isolate();
      v8::Isolate::Scope isolateScope(isolate);
      v8::HandleScope handleScope(isolate);

      v8::Local<v8::Context> context = isolate->GetCurrentContext();
      v8::Context::Scope contextScope(context);

      // Create a V8 event object using the proper Event wrapper
      auto eventObject = script_bindings::event_bindings::MakeEvent(isolate, event);

      // Call the compiled handler function with the event object
      v8::TryCatch tryCatch(isolate);
      v8::Local<v8::Value> argv[] = {eventObject};
      v8::Local<v8::Value> result;

      // Use global as 'this' context for now - ideally should be the element
      v8::Local<v8::Object> global = context->Global();

      if (!handlerFunction->Call(context, global, 1, argv).ToLocal(&result))
      {
        if (tryCatch.HasCaught())
        {
          v8::Local<v8::Message> message = tryCatch.Message();
          v8::String::Utf8Value messageUtf8(isolate, message->Get());
          cerr << "Failed to execute " << type << " handler: " << *messageUtf8 << endl;
        }
        else
        {
          cerr << "Failed to execute " << type << " handler" << endl;
        }
      }
    }
    catch (const exception &e)
    {
      cerr << "Error executing " << type << " handler: " << e.what() << endl;
    }
  }
}
