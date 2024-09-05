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

  class Node : public enable_shared_from_this<Node>
  {
  public:
    Node();
    Node(pugi::xml_node node);
    Node(Node &other);
    virtual ~Node() = default;

  public:
    vector<shared_ptr<Node>> getChildNodes();
    shared_ptr<Node> getFirstChild();
    shared_ptr<Node> getLastChild();
    shared_ptr<Node> getParentNode();
    string getTextContent();

  public:
    bool hasChildNodes();

  protected:
    /**
     * Get the shared pointer of the current `Node` object.
     */
    template <typename T = Node>
    inline shared_ptr<T> getPtr()
    {
      return dynamic_pointer_cast<T>(shared_from_this());
    }
    /**
     * Create a new `Node` object from a `pugi::xml_node`.
     */
    shared_ptr<Node> createNode(pugi::xml_node node);
    /**
     * Print the internal `pugi::xml_node` object.
     */
    void print();
    /**
     * Reset the internal `pugi::xml_node` object.
     */
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
