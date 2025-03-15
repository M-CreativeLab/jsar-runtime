#pragma once

#include <memory>
#include <concepts>

#include "./node.hpp"

namespace dom
{
  template <typename NodeType = Node>
    requires std::is_base_of_v<Node, NodeType> ||
             std::is_same_v<Node, NodeType>
  class NodeList : public std::vector<std::shared_ptr<NodeType>>
  {
  public:
    using std::vector<std::shared_ptr<NodeType>>::vector;

    NodeList(bool isLive) : isLive_(isLive)
    {
    }

  // TODO: Implement the live node list.

  private:
    bool isLive_ = false;
  };
}
