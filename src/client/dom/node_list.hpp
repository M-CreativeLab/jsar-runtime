#pragma once

#include <memory>
#include <concepts>

namespace dom
{
  // Forward declarations
  class Node;

  /**
   * A template class to represent a list of nodes with static and live modes.
   * 
   * @tparam NodeType The specific node type, such as `Element`, `Text`, `Node`, etc.
   */
  template <typename NodeType = Node>
    requires std::is_base_of_v<Node, NodeType> ||
             std::is_same_v<Node, NodeType>
  class NodeList : public std::vector<std::shared_ptr<NodeType>>
  {
    using std::vector<std::shared_ptr<NodeType>>::vector;

  public:
    /**
     * @returns An empty node list.
     */
    static inline NodeList<NodeType> Empty() { return NodeList<NodeType>(); }

  public:
    /**
     * Construct a new node list.
     * 
     * @param isLive `true` if the node list is live, otherwise `false`, default is `false`, namely static.
     */
    NodeList(bool isLive) : isLive_(isLive)
    {
    }

    // TODO: Implement the live node list.

  private:
    bool isLive_ = false;
  };
}
