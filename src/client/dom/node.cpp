#include "./parser.hpp"
#include <iostream>

namespace dom
{
  Node::Node() : internal(make_shared<pugi::xml_node>())
  {
  }

  Node::Node(pugi::xml_node node)
  {
    resetInternal(&node);
  }

  Node::Node(Node &other)
      : internal(other.internal),
        nodeName(other.nodeName),
        nodeType(other.nodeType)
  {
  }

  vector<shared_ptr<Node>> Node::getChildNodes()
  {
    vector<shared_ptr<Node>> childNodes;
    for (auto child : internal->children())
      childNodes.push_back(make_shared<Node>(child));
    return childNodes;
  }

  shared_ptr<Node> Node::getFirstChild()
  {
    if (!internal->empty())
      return make_shared<Node>(internal->first_child());
    else
      return nullptr;
  }

  shared_ptr<Node> Node::getLastChild()
  {
    if (!internal->empty())
      return make_shared<Node>(internal->last_child());
    else
      return nullptr;
  }

  shared_ptr<Node> Node::getParentNode()
  {
    if (!internal->empty())
      return make_shared<Node>(internal->parent());
    else
      return nullptr;
  }

  string Node::getTextContent()
  {
    auto text = internal->text();
    return text.as_string();
  }

  bool Node::hasChildNodes()
  {
    return !internal->empty();
  }

  void Node::print()
  {
    if (!internal->empty())
    {
      cout << "node(" << nodeName << "):" << endl;
      internal->print(cout);
    }
  }

  void Node::resetInternal(pugi::xml_node *nodeToSet)
  {
    if (nodeToSet != nullptr && !nodeToSet->empty())
      internal = make_shared<pugi::xml_node>(*nodeToSet);

    switch (internal->type())
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
  }
}
