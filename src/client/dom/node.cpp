#include <iostream>
#include <common/utility.hpp>

#include "./dom_parser.hpp"
#include "./document.hpp"
#include "./document_fragment.hpp"
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

  shared_ptr<Node> Node::appendChild(shared_ptr<Node> aChild)
  {
    if (aChild == nullptr)
      return nullptr;

    childNodes.push_back(aChild);
    aChild->parentNode = shared_from_this();

    // Connect the child node if the parent node is connected
    if (connected && !aChild->connected)
      aChild->connect();
    return aChild;
  }

  void Node::removeChild(shared_ptr<Node> aChild)
  {
    if (aChild == nullptr)
      throw runtime_error("Failed to remove the child node: the node to remove is null.");

    auto it = find(childNodes.begin(), childNodes.end(), aChild);
    if (it != childNodes.end())
    {
      childNodes.erase(it);
      aChild->disconnect();
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
      newChild->parentNode = shared_from_this();
      oldChild->parentNode = weak_ptr<Node>();
      oldChild->disconnect();

      // Connect the child node if the parent node is connected
      if (connected && !newChild->connected)
        newChild->connect();
    }
    return oldChild;
  }

  void Node::removeChildren()
  {
    for (auto it = childNodes.begin(); it != childNodes.end();)
    {
      auto child = *it;
      it = childNodes.erase(it);
      child->disconnect();
    }
    childNodes.clear();
  }

  void Node::replaceAll(shared_ptr<Node> newChild)
  {
    // Remove all the child nodes
    removeChildren();

    // If the new child is null, just remove all the child nodes.
    if (newChild == nullptr)
      return;

    if (Node::Is<DocumentFragment>(newChild))
    {
      // Append all the child nodes if the node is a `DocumentFragment`.
      auto fragment = Node::As<DocumentFragment>(newChild);
      for (auto child : fragment->childNodes)
        appendChild(child);
    }
    else if (Node::Is<Element>(newChild) ||
             Node::Is<Text>(newChild))
    {
      appendChild(newChild);
    }
    else
    {
      throw runtime_error("Failed to replace all the child nodes: the new child node is not a valid type.");
    }
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

    size_t childCount = childNodes.size();
    if (childCount == 1)
    {
      firstChild = childNodes[0];
      lastChild = childNodes[0];
    }
    else if (childCount == 2)
    {
      firstChild = childNodes[0];
      lastChild = childNodes[1];
    }
    else if (childCount > 2)
    {
      firstChild = childNodes[0];
      lastChild = childNodes[childCount - 1];
    }
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
    auto fragment = make_shared<DocumentFragment>(contextDocument);
    for (auto child : newChildNodes)
      fragment->appendChild(child);
    return fragment;
  }
}
