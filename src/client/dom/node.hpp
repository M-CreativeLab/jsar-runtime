#pragma once

#include <string>
#include <vector>
#include <memory>
#include "pugixml/pugixml.hpp"
#include "common/utility.hpp"
#include "common/events_v2/event_target.hpp"

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

#define NODE_EVENT_TYPES_MAP(XX) \
  XX(SelectStart, "selectstart")

#define ELEMENT_EVENT_TYPES_MAP(XX)            \
  XX(AnimationCancel, "animationcancel")       \
  XX(AnimationEnd, "animationend")             \
  XX(AnimationIteration, "animationiteration") \
  XX(AnimationStart, "animationstart")         \
  XX(AuxClick, "auxclick")                     \
  XX(BeforeInput, "beforeinput")               \
  XX(Blur, "blur")                             \
  XX(Click, "click")                           \
  XX(CompositionEnd, "compositionend")         \
  XX(CompositionStart, "compositionstart")     \
  XX(CompositionUpdate, "compositionupdate")   \
  XX(ContextMenu, "contextmenu")               \
  XX(Copy, "copy")                             \
  XX(Cut, "cut")                               \
  XX(DoubleClick, "dbclick")                   \
  XX(Input, "input")                           \
  XX(KeyDown, "keydown")                       \
  XX(KeyUp, "keyup")                           \
  XX(MouseDown, "mousedown")                   \
  XX(MouseEnter, "mouseenter")                 \
  XX(MouseLeave, "mouseleave")                 \
  XX(MouseMove, "mousemove")                   \
  XX(MouseOut, "mouseout")                     \
  XX(MouseOver, "mouseover")                   \
  XX(MouseUp, "mouseup")                       \
  XX(Paste, "paste")                           \
  XX(PointerCancel, "pointercancel")           \
  XX(PointerDown, "pointerdown")               \
  XX(PointerEnter, "pointerenter")             \
  XX(PointerLeave, "pointerleave")             \
  XX(PointerMove, "pointermove")               \
  XX(PointerOut, "pointerout")                 \
  XX(PointerOver, "pointerover")               \
  XX(PointerUp, "pointerup")                   \
  XX(Scroll, "scroll")                         \
  XX(ScrollEnd, "scrollend")

#define HTMLELEMENT_EVENT_TYPES_MAP(XX) \
  XX(BeforeToggle, "beforetoggle")      \
  XX(Cancel, "cancel")                  \
  XX(Change, "change")                  \
  XX(Drag, "drag")                      \
  XX(DragEnd, "dragend")                \
  XX(DragEnter, "dragenter")            \
  XX(DragLeave, "dragleave")            \
  XX(DragOver, "dragover")              \
  XX(DragStart, "dragstart")            \
  XX(Drop, "drop")                      \
  XX(Error, "error")                    \
  XX(Load, "load")                      \
  XX(Toggle, "toggle")

#define DOCUMENT_EVENT_TYPES_MAP(XX)       \
  XX(DOMContentLoaded, "domcontentloaded") \
  XX(FullscreenChange, "fullscreenchange") \
  XX(FullscreenError, "fullscreenerror")   \
  XX(SelectionChange, "selectionchange")   \
  XX(VisibilityChange, "visibilitychange")

  enum class DOMEventType
  {
#define XX(eventType, _) \
  eventType,
    NODE_EVENT_TYPES_MAP(XX)        // `Node` Events
    ELEMENT_EVENT_TYPES_MAP(XX)     // `Element` Events
    HTMLELEMENT_EVENT_TYPES_MAP(XX) // `HTMLElement` Events
    DOCUMENT_EVENT_TYPES_MAP(XX)    // `Document` Events
#undef XX
  };

  /**
   * Convert the event type string such as "click" to the `DOMEventType` enum.
   *
   * @param typeStr The event type string.
   * @returns The `DOMEventType` enum.
   */
  inline DOMEventType StringToEventType(std::string typeStr)
  {
#define XX(eventType, eventName)         \
  if (ToLowerCase(typeStr) == eventName) \
    return DOMEventType::eventType;

    NODE_EVENT_TYPES_MAP(XX)
    ELEMENT_EVENT_TYPES_MAP(XX)
    HTMLELEMENT_EVENT_TYPES_MAP(XX)
    DOCUMENT_EVENT_TYPES_MAP(XX)
#undef XX

    throw std::invalid_argument("Invalid event type string: " + typeStr);
  }

  /**
   * Convert the `DOMEventType` enum to the event type string such as "click".
   *
   * @param eventType The `DOMEventType` enum.
   * @returns The event type string.
   */
  inline std::string EventTypeToString(DOMEventType eventType)
  {
    switch (eventType)
    {
#define XX(eventType, eventName) \
  case DOMEventType::eventType:  \
    return eventName;

      NODE_EVENT_TYPES_MAP(XX)
      ELEMENT_EVENT_TYPES_MAP(XX)
      HTMLELEMENT_EVENT_TYPES_MAP(XX)
      DOCUMENT_EVENT_TYPES_MAP(XX)
#undef XX

    default:
      throw std::invalid_argument("Invalid event type: " + std::to_string(static_cast<int>(eventType)));
    }
  }

  using Event = events_comm::TrEvent<DOMEventType>;
  using EventListener = events_comm::TrEventListener<DOMEventType, Event>;

  class Document;
  class Node : public events_comm::TrEventTarget<DOMEventType>,
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
