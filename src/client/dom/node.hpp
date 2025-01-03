#pragma once

#include <string>
#include <vector>
#include <memory>
#include <pugixml/pugixml.hpp>
#include <common/utility.hpp>
#include <common/events_v2/event_target.hpp>
#include <client/macros.h>
#include "./dom_event_target.hpp"

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

  class Document;
  class Node : public DOMEventTarget,
               public enable_shared_from_this<Node>
  {
  public:
    /**
     * Create a new `Node` object from a `pugi::xml_node`.
     */
    static shared_ptr<Node> CreateNode(pugi::xml_node node, weak_ptr<Document> ownerDocument);

  public:
    /**
     * Create an empty `Node` object.
     */
    Node(NodeType nodeType, string nodeName, optional<weak_ptr<Document>> ownerDocument);
    /**
     * Create a new `Node` object from a `pugi::xml_node`.
     */
    Node(pugi::xml_node node, weak_ptr<Document> ownerDocument);
    virtual ~Node() = default;

  public:
    shared_ptr<Node> appendChild(shared_ptr<Node> aChild);
    inline vector<shared_ptr<Node>> getChildNodes() { return childNodes; }
    inline shared_ptr<Node> getFirstChild() { return firstChild; }
    inline shared_ptr<Node> getLastChild() { return lastChild; }
    inline shared_ptr<Node> getParentNode() { return parentNode; }
    string getTextContent();

  public:
    /**
     * Returns if this node has any child nodes.
     *
     * @returns True if the node has child nodes, otherwise false.
     */
    inline bool hasChildNodes() { return childNodes.size() > 0; }

  protected:
    /**
     * Get the shared pointer of the current `Node` object.
     */
    template <typename T = Node>
    inline shared_ptr<T> getPtr()
    {
      return dynamic_pointer_cast<T>(this->shared_from_this());
    }
    /**
     * Get the weak pointer of the current `Node` object.
     */
    template <typename T = Node>
    inline weak_ptr<T> getWeakPtr()
    {
      return dynamic_pointer_cast<T>(this->shared_from_this());
    }
    void resetFrom(shared_ptr<pugi::xml_node> node, weak_ptr<Document> ownerDocument);
    /**
     * Iterate all the child nodes of the current node including the child nodes of the child nodes.
     *
     * @param callback The callback function that will be called for each child node, return false to stop the iteration.
     */
    void iterateChildNodes(function<bool(shared_ptr<Node>)> callback)
    {
      for (auto childNode : childNodes)
      {
        if (callback(childNode))
          childNode->iterateChildNodes(callback);
      }
    }
    /**
     * Print the internal `pugi::xml_node` object.
     *
     * @param showTree If true, the tree will be printed.
     */
    void print(bool showTree = true);
    /**
     * Connect the node to the relevant context object.
     */
    virtual void connect();
    /**
     * Load the specific node, the stage "load" will be called after all the nodes in the DOM tree are connected.
     */
    virtual void load();
    /**
     * This method is called when the internal `pugi::xml_node` object is updated.
     */
    virtual void onInternalUpdated() {}

  private:
    void updateFromDocument(optional<weak_ptr<Document>> document);
    void updateFromInternal();
    void updateTreeFromInternal();

  public:
    /**
     * The Node's document base URI.
     */
    string baseURI;
    /**
     * A boolean value that is true if the node is connected to its relevant context object, and false if not.
     */
    bool connected;
    /**
     * A string containing the name of the `Node`.
     */
    string nodeName;
    /**
     * An `unsigned short` representing the type of the node.
     */
    NodeType nodeType;
    /**
     * Returns the `Document` that this node belongs to. If the node is itself a document, returns null.
     */
    optional<weak_ptr<Document>> ownerDocument = nullopt;
    /**
     * Returns or sets the textual content of an element and all its descendants.
     */
    string textContent;
    /**
     * The first child of the node.
     */
    shared_ptr<Node> firstChild;
    /**
     * The last child of the node.
     */
    shared_ptr<Node> lastChild;
    shared_ptr<Node> parentNode;
    vector<shared_ptr<Node>> childNodes;

  protected:
    shared_ptr<pugi::xml_node> internal;
  };
}
