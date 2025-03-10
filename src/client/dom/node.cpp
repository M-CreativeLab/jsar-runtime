#include <iostream>
#include <common/utility.hpp>

#include "./dom_parser.hpp"
#include "./document.hpp"
#include "./element.hpp"
#include "./text.hpp"

namespace dom
{
  using namespace std;

  shared_ptr<Node> Node::CreateNode(pugi::xml_node node, shared_ptr<Document> ownerDocument)
  {
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

  // textContent() returns the text content of the node and its descendants.
  const std::string Node::textContent() const
  {
    if (internal->type() == pugi::xml_node_type::node_pcdata)
      return std::string(internal->value());
    if (internal->type() == pugi::xml_node_type::node_cdata)
      return std::string(internal->child_value());

    string resultStr;
    for (auto child : childNodes)
    {
      if (child->nodeType == NodeType::TEXT_NODE)
        resultStr += child->textContent();
    }
    return resultStr;
  }

  // TODO: Implement the set text content method.
  void Node::textContent(const std::string &value)
  {
    throw std::runtime_error("The textContent property writable is not implemented yet.");
  }

  Document &Node::getOwnerDocumentChecked()
  {
    assert(ownerDocument.has_value() && "The owner document is not found.");
    return *ownerDocument.value().lock();
  }

  std::shared_ptr<Document> Node::getOwnerDocumentReference(bool force)
  {
    std::shared_ptr<dom::Document> ref = nullptr;
    if (ownerDocument.has_value())
      ref = ownerDocument.value().lock();

    if (ref == nullptr)
    {
      if (force)
        throw std::runtime_error("The owner document is not found.");
      return nullptr;
    }
    else
    {
      return ref;
    }
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
    {
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
    }

    // Connect the children from the parent.
    for (auto child : childNodes)
      child->connect();
  }

  void Node::disconnect()
  {
    // Disconnect from the leaf nodes first
    for (auto child : childNodes)
      child->disconnect();

    connected = false;
    depthInTree = nullopt;
    renderable = false;
  }

  void Node::load()
  {
    for (auto child : childNodes)
      child->load();
  }

  void Node::updateFieldsFromDocument(optional<shared_ptr<Document>> maybeDocument)
  {
    if (TR_UNLIKELY(!maybeDocument.has_value()))
      return;

    auto document = maybeDocument.value();
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
      childNodes.push_back(childNode);
      childNode->parentNode = shared_from_this();
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
}
