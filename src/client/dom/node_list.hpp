#pragma once

#include <memory>
#include <vector>

namespace dom
{
  // Forward declarations
  class Node;

  /**
   * The base class that provides the common interface for all node lists.
   */
  class NodeListApi
  {
  public:
    virtual ~NodeListApi() = default;

  public:
    // If this node list is live, it will be updated automatically when the document changes.
    virtual bool isLive() const = 0;

    // Returns the node at the specified index in the node list.
    virtual std::shared_ptr<Node> item(unsigned int index) const = 0;
    virtual unsigned int length() const = 0;

    // Auto-cast to a vector of shared pointers to Node.
    operator std::vector<std::shared_ptr<Node>>() const
    {
      std::vector<std::shared_ptr<Node>> nodes;
      for (unsigned int i = 0; i < length(); ++i)
      {
        auto node = item(i);
        if (node)
          nodes.push_back(node);
      }
      return nodes;
    }
  };

  /**
   * A template class to represent a list of nodes with static and live modes.
   *
   * @tparam NodeType The specific node type, such as `Element`, `Text`, `Node`, etc.
   */
  template <typename NodeType = Node>
    requires std::is_base_of_v<Node, NodeType> || std::is_same_v<Node, NodeType>
  class NodeList : public std::vector<std::shared_ptr<NodeType>>,
                   public NodeListApi
  {
    using T = NodeList<NodeType>;
    using std::vector<std::shared_ptr<NodeType>>::vector;

  public:
    /**
     * @returns An empty node list.
     */
    static inline NodeList<NodeType> Empty()
    {
      return NodeList<NodeType>();
    }

  public:
    /**
     * Construct a new node list.
     *
     * @param isLive `true` if the node list is live, otherwise `false`, default is `false`, namely static.
     */
    NodeList(bool isLive)
        : is_live_(isLive)
    {
    }

    // TODO: Implement the live node list.

  public:
    bool isLive() const override
    {
      return is_live_;
    }

    unsigned int length() const override
    {
      return this->size();
    }
    std::shared_ptr<Node> item(unsigned int index) const override;

  private:
    bool is_live_ = false;
  };
}
