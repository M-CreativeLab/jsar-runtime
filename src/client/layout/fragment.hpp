#pragma once

#include <glm/glm.hpp>
#include <crates/bindings.layout.hpp>
#include <client/dom/geometry/dom_rect.hpp>

#include "./geometry/rect.hpp"

namespace client_layout
{
  class Fragment
  {
    friend class TaffyBasedFormattingContext;
    friend class InlineFormattingContext;

  public:
    Fragment() = default;
    Fragment(const dom::geometry::DOMRect &rect)
        : rect_(rect),
          z_(0.0f),
          depth_(1.0f),
          border_(0.0f, 0.0f, 0.0f, 0.0f),
          padding_(0.0f, 0.0f, 0.0f, 0.0f)
    {
    }

  public:
    const dom::geometry::DOMRect &rect() const { return rect_; }

    // Position
    inline glm::vec3 xyz() const { return glm::vec3(rect_.x(), rect_.y(), 0); }
    inline float left() const { return rect_.x(); }
    inline float top() const { return rect_.y(); }
    inline float z() const { return z_; }

    // Returns a new `Fragment` object which moves the child fragment based on current fragment.
    // Such as a parent `Fragment(20, 0, 100, 100)`, moves the child `Fragment(10, 10, 50, 50)`, it will produce a new
    // `Fragment(30, 10, 50, 50)`.
    //
    // This method is used to calculate the position of the child fragment based on the parent fragment.
    const Fragment position(const Fragment &childFragment) const;

    // Size
    inline float width() const { return rect_.width(); }
    inline float height() const { return rect_.height(); }
    inline float depth() const { return depth_; }

    // Border
    inline const geometry::Rect<float> &border() const { return border_; }
    inline const geometry::Rect<float> &padding() const { return padding_; }

  private:
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

    friend std::ostream &operator<<(std::ostream &os, const Fragment &fragment)
    {
      os << "Fragment(" << fragment.rect_.x() << ", " << fragment.rect_.y() << ", "
         << fragment.rect_.width() << ", " << fragment.rect_.height() << ")";
      return os;
    }

  private:
    dom::geometry::DOMRect rect_;
    float z_ = 0.0f;
    float depth_ = 1.0f;
    geometry::Rect<float> border_ = {0.0f, 0.0f, 0.0f, 0.0f};
    geometry::Rect<float> padding_ = {0.0f, 0.0f, 0.0f, 0.0f};
  };
}
