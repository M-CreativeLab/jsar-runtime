#include <iostream>
#include <common/utility.hpp>

#include "./dom_parser.hpp"
#include "./document.hpp"
#include "./document_fragment.hpp"
#include "./mutation_observer.hpp"
#include "./element.hpp"
#include "./text.hpp"
#include "./scene_object.hpp"

namespace dom
{
  using namespace std;

  shared_ptr<Node> Node::CreateNode(pugi::xml_node node, shared_ptr<Document> ownerDocument)
  {
    assert(ownerDocument != nullptr && "The owner document is not set when creating a node.");

    shared_ptr<Node> newNode = nullptr;
    switch (node.type())
    {
    case pugi::xml_node_type::node_element:
      newNode = dynamic_pointer_cast<Node>(Element::CreateElement(node, ownerDocument));
      break;
    case pugi::xml_node_type::node_pcdata:
      newNode = dynamic_pointer_cast<Node>(Text::CreateText(node, ownerDocument));
      break;
    case pugi::xml_node_type::node_null:
    case pugi::xml_node_type::node_document:
    case pugi::xml_node_type::node_doctype:
    case pugi::xml_node_type::node_declaration:
      // Skip to create the above types of nodes
      break;
    default:
      newNode = make_shared<Node>(node, ownerDocument);
      break;
    }

    // Initialize the node
    newNode->updateTree();
    return newNode;
  }

  Node::Node(NodeType nodeType, string nodeName, optional<shared_ptr<Document>> ownerDocument)
      : DOMEventTarget(),
        uid(NodeIdGenerator.get()),
        internal(make_shared<pugi::xml_node>()),
        connected(false),
        nodeType(nodeType),
        nodeName(ToLowerCase(nodeName))
  {
    updateFieldsFromDocument(ownerDocument);
  }

  Node::Node(pugi::xml_node node, shared_ptr<Document> ownerDocument)
      : DOMEventTarget(),
        uid(NodeIdGenerator.get()),
        internal(make_shared<pugi::xml_node>(node)), connected(false)
  {
    updateFieldsFromInternal();
    updateFieldsFromDocument(ownerDocument);
  }

  Node::Node(const Node &other)
      : DOMEventTarget(),
        uid(NodeIdGenerator.get()),
        internal(other.internal),
        baseURI(other.baseURI),
        connected(false),
        nodeName(other.nodeName),
        nodeType(other.nodeType),
        ownerDocument(other.ownerDocument),
        renderable(other.renderable)
  {
  }

  shared_ptr<Node> Node::appendChild(shared_ptr<Node> aChild)
  {
    if (aChild == nullptr)
      return nullptr;

    if (Node::Is<DocumentFragment>(aChild))
    {
      // Append all the child nodes if the node is a `DocumentFragment`.
      auto fragment = Node::As<DocumentFragment>(aChild);
      for (auto child : fragment->childNodes)
        appendChild(child);
      return getOwnerDocumentChecked().createDocumentFragment();
    }
    else if (Node::Is<Element>(aChild) ||
             Node::Is<Text>(aChild))
    {
      childNodes.push_back(aChild);
      childAddedCallback(aChild);
      return aChild;
    }
    else
    {
      throw runtime_error("Failed to append the child: the new child node is not a DocumentFragment, Text or Element.");
    }
  }

  void Node::removeChild(shared_ptr<Node> aChild)
  {
    if (aChild == nullptr)
      throw runtime_error("Failed to remove the child node: the node to remove is null.");

    auto it = find(childNodes.begin(), childNodes.end(), aChild);
    if (it != childNodes.end())
    {
      childNodes.erase(it);
      childRemovedCallback(aChild);
    }
  }

  shared_ptr<Node> Node::replaceChild(shared_ptr<Node> newChild, shared_ptr<Node> oldChild)
  {
    if (newChild == nullptr || oldChild == nullptr)
      return nullptr;

    auto it = find(childNodes.begin(), childNodes.end(), oldChild);
    if (it != childNodes.end())
    {
      *it = newChild;
      childReplacedCallback(newChild, oldChild);
    }
    return oldChild;
  }

  std::shared_ptr<Node> Node::insertBefore(std::shared_ptr<Node> newChild, std::shared_ptr<Node> refChild)
  {
    if (newChild == nullptr)
      return nullptr;

    // If this is `null`, then `newChild` is inserted at the end of node's child nodes.
    if (refChild == nullptr)
    {
      appendChild(newChild);
      return newChild;
    }

    auto it = find(childNodes.begin(), childNodes.end(), refChild);
    if (it != childNodes.end())
    {
      childNodes.insert(it, newChild);
      childAddedCallback(newChild);
      return newChild;
    }
    return nullptr;
  }

  void Node::removeChildren()
  {
    for (auto it = childNodes.begin(); it != childNodes.end();)
    {
      auto child = *it;
      it = childNodes.erase(it);
      childRemovedCallback(child);
    }
    childNodes.clear();
  }

  void Node::replaceAll(shared_ptr<Node> newChild)
  {
    removeChildren();
    appendChild(newChild);
  }

  std::shared_ptr<Node> Node::cloneNode(bool deep)
  {
    assert(nodeType != NodeType::DOCUMENT_NODE && "The document node cannot be cloned.");

    std::shared_ptr<dom::Node> cloned = nullptr;
    // TODO: support the other node types
    if (nodeType == NodeType::ELEMENT_NODE)
      cloned = Element::CloneElement(shared_from_this());
    else if (nodeType == NodeType::TEXT_NODE)
      cloned = Text::CloneText(shared_from_this());
    else
      cloned = make_shared<Node>(*this);

    // Each branch should have the cloned node
    assert(cloned != nullptr && "Failed to clone the node.");

    // Clone the children if deep is true
    if (deep)
    {
      for (auto child : childNodes)
        cloned->appendChild(child->cloneNode(true));
    }
    return cloned;
  }

  shared_ptr<Element> Node::getParentElement() const
  {
    shared_ptr<Node> parentNode = getParentNode();
    if (parentNode == nullptr)
      return nullptr;

    if (Node::Is<Element>(parentNode))
      return Node::As<Element>(parentNode);
    else
      return nullptr;
  }

  NodeList<const Node> Node::getAncestors(bool inclusiveSelf, function<bool(const Node &)> ancestorsFilter) const
  {
    NodeList<const Node> ancestors;
    if (inclusiveSelf == true)
      ancestors.push_back(shared_from_this());

    shared_ptr<const Node> parent = getParentNode();
    while (parent != nullptr)
    {
      if (ancestorsFilter == nullptr || ancestorsFilter(*parent))
        ancestors.push_back(parent);
      parent = parent->getParentNode();
    }
    return ancestors;
  }

  // textContent() returns the text content of the node and its descendants.
  const string Node::textContent() const
  {
    if (internal->type() == pugi::xml_node_type::node_pcdata)
      return string(internal->value());
    if (internal->type() == pugi::xml_node_type::node_cdata)
      return string(internal->child_value());

    string resultStr;
    for (auto child : childNodes)
    {
      if (child->nodeType == NodeType::TEXT_NODE)
        resultStr += child->textContent();
    }
    return resultStr;
  }

  // TODO: Implement the set text content method.
  void Node::textContent(const string &value)
  {
    throw runtime_error("The textContent property writable is not implemented yet.");
  }

  Document &Node::getOwnerDocumentChecked()
  {
    assert(ownerDocument.has_value() && "The owner document is not found.");
    return *ownerDocument.value().lock();
  }

  shared_ptr<Document> Node::getOwnerDocumentReference()
  {
    shared_ptr<dom::Document> ref = nullptr;
    if (ownerDocument.has_value())
      ref = ownerDocument.value().lock();
    return ref;
  }

  void Node::resetFrom(shared_ptr<pugi::xml_node> node, shared_ptr<Document> ownerDocument)
  {
    internal = node;
    updateFieldsFromInternal();
    updateFieldsFromDocument(ownerDocument);
    updateTree();
  }

  void Node::print(bool showTree)
  {
    if (!internal->empty())
    {
      cout << "node(" << nodeName << "):" << endl;
      if (showTree)
        internal->print(cout);
    }
    else
    {
      cout << "node(Empty)" << endl;
    }
  }

  void Node::connect()
  {
    connected = true;
    connectedCallback();

    // Connect the children from the parent.
    for (auto child : childNodes)
      child->connect();

    afterConnectedCallback();
  }

  void Node::disconnect()
  {
    // Disconnect from the leaf nodes first
    for (auto child : childNodes)
      child->disconnect();

    // Disconnect the node
    connected = false;
    disconnectedCallback();
  }

  void Node::load()
  {
    beforeLoadedCallback();

    for (auto child : childNodes)
      child->load();

    afterLoadedCallback();
  }

  bool Node::addMutationObserver(shared_ptr<MutationObserver> observer)
  {
    if (TR_UNLIKELY(observer == nullptr))
      return false;

    auto it = find(mutationObservers.begin(), mutationObservers.end(), observer);
    if (it == mutationObservers.end())
    {
      mutationObservers.push_back(observer);
      return true;
    }
    else
    {
      return false;
    }
  }

  bool Node::removeMutationObserver(shared_ptr<MutationObserver> observer)
  {
    if (TR_UNLIKELY(observer == nullptr))
      return false;

    auto it = find(mutationObservers.begin(), mutationObservers.end(), observer);
    if (it != mutationObservers.end())
    {
      mutationObservers.erase(it);
      return true;
    }
    else
    {
      return false;
    }
  }

  bool Node::hasMutationObserver(std::function<bool(const MutationObserver &observer)> filter) const
  {
    if (mutationObservers.size() == 0)
      return false;

    bool hasObserver = false;
    for (auto observer : mutationObservers)
    {
      if (filter == nullptr || filter(*observer))
      {
        hasObserver = true;
        break;
      }
    }
    return hasObserver;
  }

  size_t Node::notifyMutationObservers(MutationRecord record)
  {
    auto isInterestedAncestors = [](const MutationObserver &observer) -> bool
    {
      // Fast filter the interested mutation observers to avoid the unnecessary checking.
      // Complete checking the interested mutation observers will be in the `queueRecord` method.
      return observer.isSubtreeObserved();
    };
    NodeList<const Node> inclusiveAncestors = getAncestors(true, [&isInterestedAncestors](const Node &node)
                                                           { return node.hasMutationObserver(isInterestedAncestors); });

    // Get the interested mutation observers from the ancestors
    vector<shared_ptr<MutationObserver>> interestedObservers;
    for (auto ancestor : inclusiveAncestors)
    {
      for (auto observer : ancestor->mutationObservers)
        interestedObservers.push_back(observer);
    }

    // Notify the mutation observers

    if (interestedObservers.empty())
      return 0;

    size_t notifiedCount = 0;
    for (auto observer : interestedObservers)
    {
      if (observer->queueRecord(record))
      {
        notifiedCount++;
      }
      else
      {
        cerr << "Failed to queue a new record" << endl;
        cerr << " record: " << record << endl;
      }
    }
    return notifiedCount;
  }

  void Node::childAddedCallback(shared_ptr<Node> child)
  {
    auto self = shared_from_this();
    child->parentNode = self;
    notifyMutationObservers(MutationRecord::OnAddChild(self, child));

    // If the parent node is connected, connect the child node as well.
    if (connected && !child->connected)
      child->connect();
  }

  void Node::childRemovedCallback(shared_ptr<Node> child)
  {
    auto self = shared_from_this();
    child->parentNode.reset();
    notifyMutationObservers(MutationRecord::OnRemoveChild(self, child));

    // Disconnect a child node no matter what the parent node is connected or not.
    child->disconnect();
  }

  void Node::childReplacedCallback(shared_ptr<Node> newChild, shared_ptr<Node> oldChild)
  {
    auto self = shared_from_this();
    newChild->parentNode = self;
    oldChild->parentNode.reset();
    notifyMutationObservers(MutationRecord::OnReplaceChild(self, newChild, oldChild));

    // Disconnect the old child node and connect the new child node if the parent node is connected.
    oldChild->disconnect();
    if (connected && !newChild->connected)
      newChild->connect();
  }

  void Node::connectedCallback()
  {
    assert(connected == true && "The node is not connected.");

    // Update the depth value when connected.
    auto parent = parentNode.lock();
    if (parent != nullptr)
    {
      depthInTree = parent->depthInTree.value_or(0) + 1;
      if (parent->renderable == true)
        renderable = true;
    }
    else
      depthInTree = 0;

    // Update the owner document
    if (parent != nullptr)
    {
      auto parentOwnerDocument = parent->getOwnerDocumentReference();
      if (parentOwnerDocument != nullptr)
        updateFieldsFromDocument(parentOwnerDocument);
    }
  }

  void Node::afterConnectedCallback()
  {
    // The default implementation does nothing.
  }

  void Node::disconnectedCallback()
  {
    assert(connected == false && "The node is connected.");
    depthInTree = nullopt;
    renderable = false;
    parentNode.reset();
  }

  void Node::beforeLoadedCallback()
  {
    // The default implementation does nothing.
  }

  void Node::afterLoadedCallback()
  {
    // The default implementation does nothing.
  }

  void Node::onInternalUpdated()
  {
    // The default implementation does nothing.
  }

  void Node::updateFieldsFromDocument(optional<shared_ptr<Document>> maybeDocument)
  {
    if (TR_UNLIKELY(!maybeDocument.has_value()))
      return;

    shared_ptr<Document> document = maybeDocument.value();
    if (TR_UNLIKELY(document == nullptr))
      return;

    baseURI = document->baseURI;
    if (nodeType != NodeType::DOCUMENT_NODE)
      ownerDocument = document;
    else
      ownerDocument = nullopt;
  }

  void Node::updateFieldsFromInternal()
  {
    auto internalType = internal->type();
    switch (internalType)
    {
    case pugi::xml_node_type::node_document:
      nodeType = NodeType::DOCUMENT_NODE;
      nodeName = "#document";
      break;
    case pugi::xml_node_type::node_element:
      nodeType = NodeType::ELEMENT_NODE;
      nodeName = internal->name();
      break;
    case pugi::xml_node_type::node_pcdata:
      nodeType = NodeType::TEXT_NODE;
      nodeName = "#text";
      break;
    case pugi::xml_node_type::node_cdata:
      nodeType = NodeType::CDATA_SECTION_NODE;
      nodeName = "#cdata-section";
      break;
    case pugi::xml_node_type::node_comment:
      nodeType = NodeType::COMMENT_NODE;
      nodeName = "#comment";
      break;
    case pugi::xml_node_type::node_pi:
      nodeType = NodeType::PROCESSING_INSTRUCTION_NODE;
      nodeName = "#processing-instruction";
      break;
    case pugi::xml_node_type::node_declaration:
    case pugi::xml_node_type::node_doctype:
      nodeType = NodeType::DOCUMENT_TYPE_NODE;
      nodeName = "#document-type";
      break;
    case pugi::xml_node_type::node_null:
    default:
      nodeType = NodeType::NULL_NODE;
      break;
    }

    // Trigger the internal updated event
    onInternalUpdated();
  }

  void Node::updateTree()
  {
    childNodes.clear();
    shared_ptr<Document> childOwnerDocument = (nodeType != NodeType::DOCUMENT_NODE && ownerDocument.has_value())
                                                  ? ownerDocument.value().lock()
                                                  : getPtr<Document>();
    assert(childOwnerDocument != nullptr && "The owner document is not set.");

    for (auto child : internal->children())
    {
      auto childNode = CreateNode(child, childOwnerDocument);
      if (childNode != nullptr)
      {
        childNodes.push_back(childNode);
        childNode->parentNode = shared_from_this();
      }
      else
      {
        cerr << "Failed to create node: " << child.name() << endl;
      }
    }
  }

  shared_ptr<Node> Node::firstChild() const
  {
    if (childNodes.empty())
      return nullptr;
    return childNodes.front();
  }

  shared_ptr<Node> Node::lastChild() const
  {
    if (childNodes.empty())
      return nullptr;
    return childNodes.back();
  }

  shared_ptr<Node> Node::previousSibling() const
  {
    if (parentNode.expired())
      return nullptr;

    auto parent = parentNode.lock();
    if (parent == nullptr)
      return nullptr;

    shared_ptr<Node> previous = nullptr;
    for (auto child : parent->childNodes)
    {
      if (child == shared_from_this())
        break;
      previous = child;
    }
    return previous;
  }

  shared_ptr<Node> Node::nextSibling() const
  {
    if (parentNode.expired())
      return nullptr;

    auto parent = parentNode.lock();
    if (parent == nullptr)
      return nullptr;

    bool found = false;
    for (auto child : parent->childNodes)
    {
      if (found)
        return child;
      if (child == shared_from_this())
        found = true;
    }
    return nullptr;
  }

  string SerializeFragment(shared_ptr<Node> node, bool wellFormed)
  {
    if (TR_UNLIKELY(node == nullptr))
      return "";

    // 1. Let context document be node's node document.
    shared_ptr<Document> contextDocument;
    if (Node::Is<Document>(node))
      contextDocument = dynamic_pointer_cast<Document>(node);
    else
    {
      if (!node->ownerDocument.has_value())
        return "";
      contextDocument = node->ownerDocument.value().lock();
      assert(contextDocument != nullptr && "The context document is not set.");
    }

    // 2. If context document is an HTML document, return the result of HTML fragment serialization algorithm with node,
    //    `false`, and « ».
    if (Node::Is<HTMLDocument>(contextDocument))
      return HTMLDocument::SerializeFragment(node, false);

    // 3. Return the XML serialization of node given require well-formed.
    return XMLDocument::SerializeFragment(node, wellFormed);
  }

  shared_ptr<DocumentFragment> ParseFragment(shared_ptr<Element> contextElement, const string &markup)
  {
    if (TR_UNLIKELY(contextElement == nullptr))
      return nullptr;

    // 1. Let context document be node's node document.
    shared_ptr<Document> contextDocument;
    if (Node::Is<Document>(contextElement))
      contextDocument = dynamic_pointer_cast<Document>(contextElement);
    else
    {
      if (!contextElement->ownerDocument.has_value())
        return nullptr;
      contextDocument = contextElement->ownerDocument.value().lock();
      assert(contextDocument != nullptr && "The context document is not set.");
    }

    vector<shared_ptr<Node>> newChildNodes;
    if (Node::Is<HTMLDocument>(contextDocument))
      newChildNodes = HTMLDocument::ParseFragment(contextElement, markup, false);
    else
      newChildNodes = XMLDocument::ParseFragment(contextElement, markup);

    // Append the new child nodes to a document fragment
    auto fragment = contextDocument->createDocumentFragment();
    for (auto child : newChildNodes)
      fragment->appendChild(child);
    return fragment;
  }
}
