#pragma once

#include <concepts>
#include <string>
#include <vector>
#include <memory>
#include <pugixml/pugixml.hpp>
#include <common/utility.hpp>
#include <common/events_v2/event_target.hpp>
#include <client/macros.h>
#include "./dom_event_target.hpp"

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

  class SceneObject;
  class Document;

  class Node : public DOMEventTarget,
               public enable_shared_from_this<Node>
  {
    friend class Attr;

  public:
    /**
     * Create a new `Node` object from a `pugi::xml_node`.
     */
    static std::shared_ptr<Node> CreateNode(pugi::xml_node node, std::shared_ptr<Document> ownerDocument);

  public:
    /**
     * Create an empty `Node` object.
     */
    Node(NodeType nodeType, std::string nodeName, std::optional<std::shared_ptr<Document>> ownerDocument);
    /**
     * Create a new `Node` object from a `pugi::xml_node`.
     */
    Node(pugi::xml_node node, std::shared_ptr<Document> ownerDocument);
    virtual ~Node() = default;

  public:
    /**
     * Append a child node to the current node.
     *
     * @param aChild The child node to append.
     * @returns The appended child node.
     */
    std::shared_ptr<Node> appendChild(std::shared_ptr<Node> aChild);
    /**
     * Get the child nodes of the current node.
     *
     * @returns a vector of the child nodes.
     */
    inline std::vector<std::shared_ptr<Node>> getChildNodes() { return childNodes; }
    /**
     * Get the first child node of the current node.
     *
     * @returns a shared pointer to the first child node.
     */
    inline std::shared_ptr<Node> getFirstChild() const { return firstChild.lock(); }
    /**
     * Get the last child node of the current node.
     *
     * @returns a shared pointer to the last child node.
     */
    inline std::shared_ptr<Node> getLastChild() const { return lastChild.lock(); }
    /**
     * Get the parent node of the current node.
     *
     * @returns a shared pointer to the parent node.
     */
    inline std::shared_ptr<Node> getParentNode() const { return parentNode.lock(); }
    /**
     * @returns The text content of the node and its descendants.
     */
    const std::string textContent() const;
    /**
     * Set the text content of the node and its descendants, it will remove all the child nodes and replace them with a single text node.
     *
     * @param value The text content to set.
     */
    void textContent(const std::string &value);
    /**
     * Get the parent node as a specific node type.
     *
     * @tparam T The specific node type, such as `Element`, `Text`, etc.
     * @returns The parent node as the specific node type, or nullptr if the parent node is not the specific node type.
     */
    template <typename T>
      requires std::is_base_of_v<Node, T> || std::is_same_v<T, SceneObject>
    std::shared_ptr<T> getParentNodeAs() const
    {
      auto _parentNode = getParentNode();
      return _parentNode == nullptr
                 ? nullptr
                 : std::dynamic_pointer_cast<T>(_parentNode);
    }

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
    inline std::shared_ptr<T> getPtr()
    {
      return dynamic_pointer_cast<T>(this->shared_from_this());
    }
    /**
     * Get the weak pointer of the current `Node` object.
     */
    template <typename T = Node>
    inline std::weak_ptr<T> getWeakPtr()
    {
      return dynamic_pointer_cast<T>(this->shared_from_this());
    }
    /**
     * Get the owner document reference.
     *
     * @param force If true, the owner document will be forced to get, otherwise it will return the cached owner document.
     * @returns The owner document reference.
     */
    std::shared_ptr<Document> getOwnerDocumentReference(bool force = true);
    /**
     * Get the owner document reference as a specific document type.
     *
     * @tparam DocumentType The specific document type, such as `Document`, `HTMLDocument`, etc.
     * @param force If true, the owner document will be forced to get, otherwise it will return the cached owner document.
     * @returns The owner document reference as the specific document type.
     */
    template <typename DocumentType>
      requires std::is_base_of_v<Document, DocumentType>
    std::shared_ptr<DocumentType> getOwnerDocumentReferenceAs(bool force = true)
    {
      auto ref = std::dynamic_pointer_cast<DocumentType>(getOwnerDocumentReference(force));
      if (force && ref == nullptr)
        throw std::runtime_error("The owner document is not found.");
      return ref;
    }
    void resetFrom(std::shared_ptr<pugi::xml_node> node, std::shared_ptr<Document> ownerDocument);
    /**
     * Iterate all the child nodes of the current node including the child nodes of the child nodes.
     *
     * @param callback The callback function that will be called for each child node, return false to stop the iteration.
     */
    void iterateChildNodes(std::function<bool(std::shared_ptr<Node>)> callback)
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
    // Update the fields from the document, such as the base URI, owner document, etc.
    void updateFieldsFromDocument(std::optional<std::shared_ptr<Document>> document);
    // Update the fields from the internal `pugi::xml_node` object, such as the node type, node name, etc.
    void updateFieldsFromInternal();
    // Update the tree fields, such as the child nodes, first child, last child, etc.
    void updateTree();

  public:
    /**
     * The Node's document base URI.
     */
    std::string baseURI;
    /**
     * A boolean value that is true if the node is connected to its relevant context object, and false if not.
     */
    bool connected;
    /**
     * A string containing the name of the `Node`.
     */
    std::string nodeName;
    /**
     * An `unsigned short` representing the type of the node.
     */
    NodeType nodeType;
    /**
     * Returns the `Document` that this node belongs to. If the node is itself a document, returns null.
     */
    std::optional<std::weak_ptr<Document>> ownerDocument = nullopt;
    /**
     * The weak reference to the first child of this node, if you need to get the shared pointer, use `getFirstChild()`.
     */
    std::weak_ptr<Node> firstChild;
    /**
     * The weak reference to the last child of this node, if you need to get the shared pointer, use `getLastChild()`.
     */
    std::weak_ptr<Node> lastChild;
    /**
     * The weak reference to the parent node of this node, if you need to get the shared pointer, use `getParentNode()`.
     */
    std::weak_ptr<Node> parentNode;
    /**
     * The child nodes of this node.
     */
    std::vector<std::shared_ptr<Node>> childNodes;

  protected:
    std::shared_ptr<pugi::xml_node> internal;
  };
}
