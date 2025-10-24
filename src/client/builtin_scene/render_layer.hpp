#pragma once

#include <functional>
#include "./ecs.hpp"

namespace endor
{
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
} // namespace endor

namespace std
{
  template <>
  struct hash<endor::builtin_scene::RenderLayer>
  {
    inline size_t operator()(const endor::builtin_scene::RenderLayer &layer) const noexcept
    {
      return hash<int>()(layer.index());
    }
  };
}
