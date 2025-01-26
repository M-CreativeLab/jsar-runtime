#pragma once

#include <optional>
#include <glm/glm.hpp>
#include <crates/bindings.hpp>

namespace client_cssom
{
  class Layout : public crates::layout::Layout
  {
    using crates::layout::Layout::Layout;

  public:
    static Layout Merge(const std::optional<Layout> &parent, const Layout &child)
    {
      if (!parent.has_value())
        return child;
      else
      {
        Layout layout = parent.value();
        layout.merge(child);
        return layout;
      }
    }

  public:
    Layout() : crates::layout::Layout(), depth_(0.0f) {};
    Layout(crates::layout::Layout &&layout, float depth = 0.0f)
        : crates::layout::Layout(layout), depth_(depth)
    {
    }

  public:
    inline float depth() const { return depth_; }
    inline glm::vec2 xy() const
    {
      return {left(), top()};
    }
    inline glm::vec3 xyz() const
    {
      return {left(), top(), depth_};
    }

  public:
    void merge(const Layout &layout);
    /**
     * Check if the layout needs to be resized with the specified width and height.
     * 
     * @param width The width to check.
     * @param height The height to check.
     * @returns Whether the layout needs to be resized.
     */
    bool needsResize(float width, float height) const;

  private:
    float depth_ = 0.0f;
  };
}
