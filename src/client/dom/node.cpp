#include <iostream>
#include "common/utility.hpp"
#include "./dom_parser.hpp"
#include "./document.hpp"
#include "./element.hpp"

namespace dom
{
  using namespace std;

  shared_ptr<Node> Node::CreateNode(pugi::xml_node node, weak_ptr<Document> ownerDocument)
  {
    shared_ptr<Node> newNode = nullptr;
    switch (node.type())
    {
    case pugi::xml_node_type::node_element:
      newNode = dynamic_pointer_cast<Node>(Element::CreateElement(node, ownerDocument));
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

  Node::Node(NodeType nodeType, string nodeName, optional<weak_ptr<Document>> ownerDocument)
      : DOMEventTarget(),
        internal(make_shared<pugi::xml_node>()),
        connected(false),
        nodeType(nodeType),
        nodeName(ToLowerCase(nodeName))
  {
    updateFieldsFromDocument(ownerDocument);
  }

  Node::Node(pugi::xml_node node, weak_ptr<Document> ownerDocument)
      : DOMEventTarget(),
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

  string Node::getTextContent()
  {
    auto text = internal->text();
    return text.as_string();
  }

  void Node::resetFrom(shared_ptr<pugi::xml_node> node, weak_ptr<Document> ownerDocument)
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
    for (auto child : childNodes)
      child->connect();
  }

  void Node::load()
  {
    for (auto child : childNodes)
      child->load();
  }

  void Node::updateFieldsFromDocument(optional<weak_ptr<Document>> maybeDocument)
  {
    if (TR_UNLIKELY(!maybeDocument.has_value()))
      return;

    auto document = maybeDocument.value();
    if (TR_UNLIKELY(document.expired()))
      return;

    baseURI = document.lock()->baseURI;
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

    if (!internal->text().empty())
      textContent = internal->text().as_string();

    // Trigger the internal updated event
    onInternalUpdated();
  }

  void Node::updateTree()
  {
    childNodes.clear();
    weak_ptr<Document> childOwnerDocument = (nodeType != NodeType::DOCUMENT_NODE && ownerDocument.has_value())
                                                ? ownerDocument.value()
                                                : getWeakPtr<Document>();
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
