#pragma once

#include <glm/glm.hpp>
#include <crates/bindings.hpp>

namespace client_cssom
{
  class Layout : public crates::layout::Layout
  {
    using crates::layout::Layout::Layout;

  public:
    Layout(crates::layout::Layout &&layout, float depth = 0.0f)
        : crates::layout::Layout(layout), depth_(depth)
    {
    }

  public:
    inline float depth() const { return depth_; }
    inline glm::vec2 xy() const
    {
      return {x(), y()};
    }
    inline glm::vec3 xyz() const
    {
      return {x(), y(), depth_};
    }

  private:
    float depth_ = 0.0f;
  };
}
