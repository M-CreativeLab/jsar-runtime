#include "./dom_parser.hpp"
#include "./document.hpp"
#include "./element.hpp"
#include <iostream>

namespace dom
{
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
    return newNode;
  }

  Node::Node() : internal(make_shared<pugi::xml_node>())
  {
  }

  Node::Node(pugi::xml_node node, weak_ptr<Document> ownerDocument)
  {
    resetInternal(&node, ownerDocument);
  }

  Node::Node(Node &other)
      : internal(other.internal),
        nodeName(other.nodeName),
        nodeType(other.nodeType),
        textContent(other.textContent),
        parentNode(other.parentNode),
        firstChild(other.firstChild),
        lastChild(other.lastChild),
        childNodes(other.childNodes),
        ownerDocument(other.ownerDocument)
  {
  }

  string Node::getTextContent()
  {
    auto text = internal->text();
    return text.as_string();
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

  void Node::resetInternal(pugi::xml_node *nodeToSet, weak_ptr<Document> fromDocument)
  {
    if (nodeToSet != nullptr && !nodeToSet->empty())
      internal = make_shared<pugi::xml_node>(*nodeToSet);
    if (!fromDocument.expired())
      baseURI = fromDocument.lock()->baseURI;

    weak_ptr<Document> childOwnerDocument;
    auto internalType = internal->type();
    if (internalType != pugi::xml_node_type::node_document)
    {
      ownerDocument = fromDocument;
      childOwnerDocument = fromDocument;
    }
    else
    {
      ownerDocument.reset();
      childOwnerDocument = getWeakPtr<Document>();
    }

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

    for (auto child : internal->children())
      childNodes.push_back(CreateNode(child, childOwnerDocument));

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
}
