#pragma once

#include <optional>
#include <glm/glm.hpp>
#include <crates/bindings.layout.hpp>
#include <client/dom/geometry/dom_rect.hpp>

#include "./geometry/rect.hpp"
#include "./geometry/viewport3d.hpp"

namespace client_layout
{
  class Fragment
  {
    friend class TaffyBasedFormattingContext;
    friend class InlineFormattingContext;

  public:
    // Create a none fragment which has a size of 0x0.
    static Fragment None() { return Fragment(dom::geometry::DOMRect(0, 0, 0, 0)); }

  public:
    Fragment() = default;
    Fragment(const dom::geometry::DOMRect &);
    // Construct a fragment from the native layout output.
    Fragment(const crates::layout2::Layout &);

  public:
    // Returns the 2d rectangle of the fragment, using the style size.
    inline dom::geometry::DOMRect getRect() const
    {
      return dom::geometry::DOMRect(position_.x, position_.y, size_.x, size_.y);
    }

    // Position
    inline glm::vec3 xyz() const { return position_; }
    inline float left() const { return position_.x; }
    inline float right() const { return position_.x + size_.x; }
    inline float top() const { return position_.y; }
    inline float bottom() const { return position_.y + size_.y; }
    inline float z() const { return position_.z; }

    // Returns a new `Fragment` object which moves the child fragment based on current fragment.
    // Such as a parent `Fragment(20, 0, 100, 100)`, moves the child `Fragment(10, 10, 50, 50)`, it will produce a new
    // `Fragment(30, 10, 50, 50)`.
    //
    // This method is used to calculate the position of the child fragment based on the parent fragment.
    const Fragment position(const Fragment &childFragment) const;

    // Move the fragment by the given x, y, z values.
    void moveBy(float x, float y, float z = 0.0f);

    // Size
    inline glm::vec3 size() const
    {
      return glm::vec3(width(),
                       height(),
                       depth());
    }
    inline float width() const { return size_.x; }
    inline float height() const { return size_.y; }
    inline float depth() const { return size_.z; }

    inline glm::vec3 contentSize() const
    {
      return glm::vec3(contentWidth(),
                       contentHeight(),
                       contentDepth());
    }
    inline float contentWidth() const { return content_size_.has_value() ? content_size_->x : size_.x; }
    inline float contentHeight() const { return content_size_.has_value() ? content_size_->y : size_.y; }
    inline float contentDepth() const { return content_size_.has_value() ? content_size_->z : size_.z; }

    // Border
    inline const geometry::Rect<float> &border() const { return border_; }
    inline const geometry::Rect<float> &padding() const { return padding_; }

    bool visibleInViewport(const geometry::Viewport3d &) const;

  private:
    inline void setSize(float width, float height, float depth)
    {
      size_ = glm::vec3(width, height, depth);
    }
    inline void setWidth(float width) { size_.x = width; }
    inline void setHeight(float height) { size_.y = height; }
    inline void setDepth(float depth) { size_.z = depth; }

    inline void setBorder(float top, float right, float bottom, float left)
    {
      border_ = geometry::Rect<float>(top, right, bottom, left);
    }
    inline void setBorder(const crates::layout2::Rect<float> &border)
    {
      setBorder(border.top(),
                border.right(),
                border.bottom(),
                border.left());
    }
    inline void setPadding(float top, float right, float bottom, float left)
    {
      padding_ = geometry::Rect<float>(top, right, bottom, left);
    }
    inline void setPadding(const crates::layout2::Rect<float> &padding)
    {
      setPadding(padding.top(),
                 padding.right(),
                 padding.bottom(),
                 padding.left());
    }

    friend std::ostream &operator<<(std::ostream &os, const Fragment &fragment);

  private:
    glm::vec3 size_ = {0.0f, 0.0f, 1.0f};
    std::optional<glm::vec3> content_size_ = std::nullopt;
    glm::vec3 position_ = {0.0f, 0.0f, 0.0f};
    geometry::Rect<float> border_ = {0.0f, 0.0f, 0.0f, 0.0f};
    geometry::Rect<float> padding_ = {0.0f, 0.0f, 0.0f, 0.0f};
  };
}
