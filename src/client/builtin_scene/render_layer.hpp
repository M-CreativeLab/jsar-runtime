#pragma once

#include <functional>
#include "./ecs.hpp"

namespace builtin_scene
{
  class RenderLayer : ecs::Component
  {
    using ecs::Component::Component;

  public:
    RenderLayer(int index = 0)
        : ecs::Component()
        , index_(index)
    {
    }

    int index() const
    {
      return index_;
    }

    operator int() const
    {
      return index_;
    }

    bool operator==(const RenderLayer &other) const
    {
      return index_ == other.index_;
    }
    bool operator!=(const RenderLayer &other) const
    {
      return index_ != other.index_;
    }
    bool operator<(const RenderLayer &other) const
    {
      return index_ < other.index_;
    }
    bool operator>(const RenderLayer &other) const
    {
      return index_ > other.index_;
    }

  private:
    int index_;
  };
}

namespace std
{
  template <>
  struct hash<builtin_scene::RenderLayer>
  {
    inline size_t operator()(const builtin_scene::RenderLayer &layer) const noexcept
    {
      return hash<int>()(layer.index());
    }
  };
}
