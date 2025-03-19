#pragma once

#include <concepts>
#include <string>
#include <vector>
#include <memory>
#include <pugixml/pugixml.hpp>
#include <idgen.hpp>
#include <common/utility.hpp>
#include <common/events_v2/event_target.hpp>
#include <client/macros.h>

#include "./dom_event_target.hpp"
#include "./node_list.hpp"

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

  // Forward declarations
  class SceneObject;
  class Element;
  class Document;
  class DocumentFragment;
  class MutationRecord;
  class MutationObserver;

  class Node : public DOMEventTarget,
               public enable_shared_from_this<Node>
  {
    friend class Document;
    friend class Attr;

  public:
    /**
     * Check if the given node is a specific type of node.
     *
     * @tparam T The specific node type, such as `Element`, `Text`, etc.
     *
     * @param node The node to check.
     * @returns `true` if the node is the specific type, otherwise `false`.
     */
    template <typename T>
      requires std::is_base_of_v<Node, T> || std::is_same_v<T, Node>
    static inline bool Is(std::shared_ptr<Node> node)
    {
      assert(node != nullptr && "The node is null.");
      return dynamic_pointer_cast<T>(node) != nullptr;
    }
    /**
     * Get the given node as a specific type of node.
     *
     * @tparam T The specific node type, such as `Element`, `Text`, etc.
     *
     * @param node The node to get.
     * @returns The node as the specific type, or nullptr if the node is not the specific type.
     */
    template <typename T>
      requires std::is_base_of_v<Node, T>
    static inline std::shared_ptr<T> As(std::shared_ptr<Node> node)
    {
      assert(node != nullptr && "The node is null.");
      return dynamic_pointer_cast<T>(node);
    }
    /**
     * Get the given node reference as a specific type of node, it will fail if the node is not the specific type.
     *
     * @tparam T The specific node type, such as `Element`, `Text`, etc.
     *
     * @param node The node to get.
     * @returns The node reference as the specific type.
     */
    template <typename T>
      requires std::is_base_of_v<Node, T>
    static inline T &AsChecked(std::shared_ptr<Node> node)
    {
      auto ptr = As<T>(node);
      assert(ptr != nullptr && "The node is not the specific type.");
      return *ptr;
    }
    /**
     * Create a new `Node` object from a `pugi::xml_node`.
     */
    static std::shared_ptr<Node> CreateNode(pugi::xml_node node, std::shared_ptr<Document> ownerDocument);

  public:
    /**
     * Create an empty `Node` object.
     * 
     * @param nodeType The type of the node.
     * @param nodeName The name of the node.
     * @param ownerDocument The owner document of the node.
     */
    Node(NodeType nodeType, std::string nodeName, std::optional<std::shared_ptr<Document>> ownerDocument);
    /**
     * Create a new `Node` object from a `pugi::xml_node`.
     * 
     * @param node The `pugi::xml_node` object to create the node.
     * @param ownerDocument The owner document of the node.
     */
    Node(pugi::xml_node node, std::shared_ptr<Document> ownerDocument);
    /**
     * Copy constructor to use for cloning the node.
     * 
     * @param other The node to copy.
     */
    Node(const Node &other);
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
     * Remove a child node from the current node.
     *
     * @param aChild The child node to remove.
     * @returns The removed child node.
     */
    void removeChild(std::shared_ptr<Node> aChild);
    /**
     * Replace a child node with a new child node.
     *
     * @param newChild The new child node to replace.
     * @param oldChild The old child node to replace.
     * @returns The replaced child node.
     */
    std::shared_ptr<Node> replaceChild(std::shared_ptr<Node> newChild, std::shared_ptr<Node> oldChild);
    /**
     * Remove all the child nodes from the current node.
     */
    void removeChildren();
    /**
     * Replace all the child nodes with a new child node.
     *
     * @param newChild The new child node to replace.
     */
    void replaceAll(std::shared_ptr<Node> newChild);
    /**
     * Insert a child node before a specific child node.
     * 
     * @param newChild The new child node to insert.
     * @param refChild The reference child node to insert before.
     * @returns The inserted child node.
     */
    std::shared_ptr<Node> insertBefore(std::shared_ptr<Node> newChild, std::shared_ptr<Node> refChild);
    /**
     * Clone the current node.
     * 
     * @returns a duplicate of the node on which this method was called.
     */
    std::shared_ptr<Node> cloneNode(bool deep);
    /**
     * Get the child nodes of the current node.
     *
     * @returns a vector of the child nodes.
     */
    inline std::vector<std::shared_ptr<Node>> getChildNodes() { return childNodes; }
    /**
     * Get the parent node of the current node.
     *
     * @returns a shared pointer to the parent node.
     */
    inline std::shared_ptr<Node> getParentNode() const { return parentNode.lock(); }
    /**
     * Get the parent `Element` node of the current node, if the node has no parent, or if that parent is not an
     * `Element`, this property returns `null`.
     * 
     * @returns The parent `Element` node or `null`.
     */
    std::shared_ptr<Element> getParentElement() const;
    /**
     * Get the ancestors of the current node.
     *
     * @param inclusiveSelf If true, the current node will be included in the ancestors.
     * @param ancestorsFilter The filter function to filter the node.
     * @returns a `NodeList` that contains the result ancestors.
     */
    NodeList<const Node> getAncestors(bool inclusiveSelf,
                                      std::function<bool(const Node &)> ancestorsFilter = nullptr) const;
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
     * Check if the current node has a specific type of parent node.
     */
    template <typename T>
      requires std::is_base_of_v<Node, T> || std::is_same_v<T, SceneObject>
    inline bool hasTypedParentNode() const
    {
      auto _parentNode = getParentNode();
      return _parentNode != nullptr && std::dynamic_pointer_cast<T>(_parentNode) != nullptr;
    }
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
    /**
     * Get the owner document reference.
     *
     * @returns The owner document reference.
     */
    Document &getOwnerDocumentChecked();
    /**
     * Get the owner document reference.
     *
     * @param force If true, the owner document will be forced to get, otherwise it will return the cached owner document.
     * @returns The owner document reference.
     */
    std::shared_ptr<Document> getOwnerDocumentReference();
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
      auto ref = std::dynamic_pointer_cast<DocumentType>(getOwnerDocumentReference());
      if (force && ref == nullptr)
        throw std::runtime_error("Could not cast this node's owner document to the specific document type.");
      return ref;
    }

  public:
    /**
     * Returns if this node has any child nodes.
     *
     * @returns True if the node has child nodes, otherwise false.
     */
    inline bool hasChildNodes() const { return childNodes.size() > 0; }
    /**
     * The `isEqualNode()` method of the `Node` interface tests whether two nodes are equal. Two nodes are equal when they have the
     * same type, defining characteristics (for elements, this would be their ID, number of children, and so forth), its attributes
     * match, and so on. The specific set of data points that must match varies depending on the types of the nodes.
     *
     * @param other The node to compare.
     * @returns `true` if the nodes are equal, otherwise `false`.
     */
    inline bool isEqualNode(const Node &other) const
    {
      // TODO: Implement the `isEqualNode` method.
      return isSameNode(other);
    }
    /**
     * It tests whether two nodes are the same (in other words, whether they reference the same object).
     *
     * @param other The node to compare.
     * @returns `true` if the nodes are the same, otherwise `false`.
     */
    inline bool isSameNode(const Node &other) const
    {
      return this->uid == other.uid;
    }
    /**
     * @returns The node's depth in the tree.
     */
    inline uint32_t depth() const
    {
      return depthInTree.value_or(0);
    }

  public:
    bool operator==(const Node &other) const { return isEqualNode(other); }

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

  public:
    /**
     * Connect the node to the relevant context object.
     */
    void connect();
    /**
     * Disconnect the node from the relevant context object.
     */
    void disconnect();
    /**
     * Load the specific node, the stage "load" will be called after all the nodes in the DOM tree are connected.
     */
    void load();

  public: // Internal public methods
    /**
     * Add a mutation observer to the node.
     *
     * @param observer The mutation observer to add.
     * @returns `true` if the mutation observer is added, otherwise `false`.
     */
    bool addMutationObserver(std::shared_ptr<MutationObserver> observer);
    /**
     * Remove a mutation observer from the node.
     *
     * @param observer The mutation observer to remove.
     * @returns `true` if the mutation observer is removed, otherwise `false`.
     */
    bool removeMutationObserver(std::shared_ptr<MutationObserver> observer);
    /**
     * Check if the node has mutation observers with the given options, this is used to get the how many mutation
     * observers are observing the node.
     *
     * @param filter The filter function to filter the mutation observer.
     * @returns `true` if the node has mutation observers, otherwise `false`.
     */
    bool hasMutationObserver(std::function<bool(const MutationObserver &observer)> filter = nullptr) const;
    /**
     * Notify a mutation record to the mutation observers, and return the number of notified mutation observers.
     *
     * @param record The mutation record to notify.
     * @returns The number of notified mutation observers.
     */
    size_t notifyMutationObservers(MutationRecord record);

  public: // Node lifecycle callbacks
    /**
     * Get called each time a child node is added.
     *
     * @param child The child node that is added.
     */
    virtual void childAddedCallback(std::shared_ptr<Node> child);
    /**
     * Get called each time a child node is removed.
     *
     * @param child The child node that is removed.
     */
    virtual void childRemovedCallback(std::shared_ptr<Node> child);
    /**
     * Get called each time a child node is replaced.
     *
     * @param newChild The new child node that is replaced.
     * @param oldChild The old child node that is replaced.
     */
    virtual void childReplacedCallback(std::shared_ptr<Node> newChild, std::shared_ptr<Node> oldChild);
    /**
     * Get called each time the node is added to the document. The specification recommends that, as far as possible,
     * developers should implement custom element setup in this callback rather than the constructor.
     */
    virtual void connectedCallback();
    /**
     * Get called when the node has been connected to the document and all the inherited connected callbacks have been
     * called, this callback is useful for the node to do some post-connection work, that depends on the connected
     * state.
     */
    virtual void afterConnectedCallback();
    /**
     * Get called each time the element is removed from the document.
     */
    virtual void disconnectedCallback();
    /**
     * Get called each time before the node is to be loaded.
     */
    virtual void beforeLoadedCallback();
    /**
     * Get called each time after the node is loaded.
     */
    virtual void afterLoadedCallback();
    /**
     * This method is called when the internal `pugi::xml_node` object is updated.
     */
    virtual void onInternalUpdated();

  private:
    // Update the fields from the document, such as the base URI, owner document, etc.
    void updateFieldsFromDocument(std::optional<std::shared_ptr<Document>> document);
    // Update the fields from the internal `pugi::xml_node` object, such as the node type, node name, etc.
    void updateFieldsFromInternal();
    // Update the tree fields, such as the child nodes, first child, last child, etc.
    void updateTree();

  public:
    /**
     * The unique identifier of the `Node`.
     */
    uint32_t uid;
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
     * The `Node` is the first child of the current node.
     */
    std::shared_ptr<Node> firstChild() const;
    /**
     * The `Node` is the last child of the current node.
     */
    std::shared_ptr<Node> lastChild() const;
    /**
     * The weak reference to the parent node of this node, if you need to get the shared pointer, use `getParentNode()`.
     */
    std::weak_ptr<Node> parentNode;
    /**
     * The child nodes of this node.
     */
    std::vector<std::shared_ptr<Node>> childNodes;
    /**
     * The `Node` immediately preceding the specified one in its parent's childNodes list.
     */
    std::shared_ptr<Node> previousSibling() const;
    /**
     * The `Node` immediately following the specified one in its parent's childNodes list.
     */
    std::shared_ptr<Node> nextSibling() const;

  protected:
    std::shared_ptr<pugi::xml_node> internal;
    std::optional<uint32_t> depthInTree = std::nullopt;
    // If this node could be rendered, `false` by default.
    bool renderable = false;
    // The mutation observers of this node.
    std::vector<std::shared_ptr<MutationObserver>> mutationObservers;

  private:
    inline static TrIdGenerator NodeIdGenerator = TrIdGenerator(0x1a);
  };

  /**
   * Serialize an `Element`, `Document` or `DocumentFragment` object to a string.
   *
   * This implements fragment serializing algorithm steps.
   *
   * see https://html.spec.whatwg.org/multipage/dynamic-markup-insertion.html#fragment-serializing-algorithm-steps
   */
  std::string SerializeFragment(std::shared_ptr<Node> node, bool wellFormed);

  /**
   * Parse the given markup string and return a `DocumentFragment` object.
   *
   * see https://html.spec.whatwg.org/multipage/dynamic-markup-insertion.html#fragment-parsing-algorithm-steps
   *
   * @param contextElement The context element to parse the markup.
   * @param markup The markup string to parse.
   * @returns The parsed `DocumentFragment` object.
   */
  std::shared_ptr<DocumentFragment> ParseFragment(std::shared_ptr<Element> contextElement, const std::string &markup);
}
