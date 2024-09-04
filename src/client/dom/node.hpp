#pragma once

#include <string>
#include <memory>
#include "pugixml/pugixml.hpp"

using namespace std;

namespace dom
{
  enum class NodeType
  {
    NULL_NODE = 0,
    ELEMENT_NODE,
    ATTRIBUTE_NODE,
    TEXT_NODE,
    CDATA_SECTION_NODE,
    ENTITY_REFERENCE_NODE,
    ENTITY_NODE,
    PROCESSING_INSTRUCTION_NODE,
    COMMENT_NODE,
    DOCUMENT_NODE,
    DOCUMENT_TYPE_NODE,
    DOCUMENT_FRAGMENT_NODE,
    NOTATION_NODE,
  };

  class Node
  {
  public:
    Node();
    Node(pugi::xml_node node);
    Node(Node &other);
    ~Node() = default;

  public:
    shared_ptr<Node> getFirstChild();
    shared_ptr<Node> getLastChild();
    shared_ptr<Node> getParentNode();

  public:
    // std::shared_ptr<Node> appendChild(std::shared_ptr<Node> node);
    // std::shared_ptr<Node> cloneNode(bool deep = false);
    // bool contains(std::shared_ptr<Node> node);
    bool hasChildNodes();

  protected:
    void print();
    void resetInternal(pugi::xml_node *nodeToSet);

  public:
    string baseURI;
    bool connected;
    /**
     * A string containing the name of the `Node`.
     */
    string nodeName;
    /**
     * An `unsigned short` representing the type of the node.
     */
    NodeType nodeType;
    string textContent;

    shared_ptr<Node> firstChild;
    shared_ptr<Node> lastChild;
    shared_ptr<Node> parentNode;

  protected:
    shared_ptr<pugi::xml_node> internal;
  };
}
