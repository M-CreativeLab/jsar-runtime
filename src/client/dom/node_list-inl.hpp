#pragma once

#include "./node.hpp"
#include "./node_list.hpp"

namespace dom
{
  template <typename NodeType>
    requires std::is_base_of_v<Node, NodeType> || std::is_same_v<Node, NodeType>
  std::shared_ptr<Node> NodeList<NodeType>::item(unsigned int index) const
  {
    if (index > this->size())
      return nullptr;

    shared_ptr<const Node> node;
    if constexpr (std::is_base_of_v<Node, NodeType>)
      node = dynamic_pointer_cast<const Node>(this->at(index));
    else
      node = this->at(index);

    return node == nullptr
             ? nullptr
             : const_pointer_cast<Node>(node);
  }
}
