#pragma once

#include <string>
#include <vector>
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
    /**
     * Create a new `Node` object from a `pugi::xml_node`.
     */
    static shared_ptr<Node> CreateNode(pugi::xml_node node);

  public:
    /**
     * Create an empty `Node` object.
     */
    Node();
    /**
     * Create a new `Node` object from a `pugi::xml_node`.
     */
    Node(pugi::xml_node node);
    Node(Node &other);
    virtual ~Node() = default;

  public:
    inline vector<shared_ptr<Node>> getChildNodes() { return childNodes; }
    inline shared_ptr<Node> getFirstChild() { return firstChild; }
    inline shared_ptr<Node> getLastChild() { return lastChild; }
    inline shared_ptr<Node> getParentNode() { return parentNode; }
    string getTextContent();

  public:
    inline bool hasChildNodes() { return childNodes.size() > 0; }

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
     * Print the internal `pugi::xml_node` object.
     * 
     * @param showTree If true, the tree will be printed.
     */
    void print(bool showTree = true);
    /**
     * Reset the internal `pugi::xml_node` object.
     * 
     * @param nodeToSet The `pugi::xml_node` object to set.
     */
    void resetInternal(pugi::xml_node *nodeToSet);
    /**
     * Connect the node to the relevant context object.
     */
    virtual void connect();

  public:
    string baseURI;
    /**
     * A boolean value that is true if the node is connected to its relevant context object, and false if not.
     */
    bool connected = false;
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
    vector<shared_ptr<Node>> childNodes;

  protected:
    shared_ptr<pugi::xml_node> internal;
  };
}
