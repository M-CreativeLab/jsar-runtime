#pragma once

#include <optional>
#include <glm/glm.hpp>
#include <crates/bindings.hpp>

namespace client_cssom
{
  class Layout : public crates::layout2::Layout
  {
    using crates::layout2::Layout::Layout;

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
    Layout() : crates::layout2::Layout(), depth_(0.0f) {};
    Layout(crates::layout2::Layout &&layout, float depth = 0.0f)
        : crates::layout2::Layout(layout), depth_(depth)
    {
    }

  public:
    inline float depth() const { return depth_; }
    inline glm::vec2 xy() const
    {
      return {left() + offsetX_,
              top() + offsetY_};
    }
    inline glm::vec3 xyz() const
    {
      return {left() + offsetX_,
              top() + offsetY_,
              depth_};
    }
    inline void setOffset(float x, float y, float z = 0.0f)
    {
      offsetX_ = x;
      offsetY_ = y;
      offsetZ_ = z;
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
    float offsetX_ = 0.0f;
    float offsetY_ = 0.0f;
    float offsetZ_ = 0.0f;
  };
}
