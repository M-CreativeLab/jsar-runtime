#pragma once

#include <iostream>
#include <glm/glm.hpp>

namespace client_layout::geometry
{
  /**
   * A rectangle with top, right, bottom, and left values.
   *
   * @tparam T The type of the rectangle values.
   */
  template <typename T>
  class Rect
  {
  public:
    Rect() = default;
    Rect(T top, T right, T bottom, T left)
        : top_(top), right_(right), bottom_(bottom), left_(left) {}

  public:
    T &top() { return top_; }
    T top() const { return top_; }
    T &right() { return right_; }
    T right() const { return right_; }
    T &bottom() { return bottom_; }
    T bottom() const { return bottom_; }
    T &left() { return left_; }
    T left() const { return left_; }

    // Move the rectangle by the given offset.
    void move(const glm::vec<2, T, glm::packed_highp> offset)
    {
      top_ += offset.y;
      right_ += offset.x;
      bottom_ += offset.y;
      left_ += offset.x;
    }

  public:
    friend std::ostream &operator<<(std::ostream &os, const Rect<T> &style)
    {
      os << "(" << style.top_ << ", " << style.right_ << ", " << style.bottom_ << ", " << style.left_ << ")";
      return os;
    }

  private:
    T top_;
    T right_;
    T bottom_;
    T left_;
  };
}
