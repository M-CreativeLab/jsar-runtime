#pragma once

#include <string>
#include <client/classes.hpp>
#include "../ecs-inl.hpp"

namespace builtin_scene::hierarchy
{
  class Element : public ecs::Component
  {
    using ecs::Component::Component;

  public:
    Element(std::string name, std::shared_ptr<dom::Node> node)
        : name(name), node(node)
    {
    }

  public:
    template <typename T>
      requires std::is_same_v<T, dom::SceneObject>
    std::shared_ptr<T> getNode() const
    {
      return std::dynamic_pointer_cast<T>(node);
    }

    template <typename T>
      requires std::is_same_v<T, dom::SceneObject>
    T& getNodeChecked() const
    {
      auto ref = getNode<T>();
      assert(ref != nullptr && "The node must be valid.");
      return *ref;
    }

  public:
    std::string name;
    std::shared_ptr<dom::Node> node;
  };
}
