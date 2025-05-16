#pragma once

namespace client_cssom::values::generics
{
  enum class Edge
  {
    kTop,
    kRight,
    kBottom,
    kLeft
  };

  template <typename T>
  class Rect
  {
  public:
    static Rect<T> All(T value)
    {
      return Rect<T>(value, value, value, value);
    }
    static Rect<T> Default()
    {
      return Rect<T>(T(), T(), T(), T());
    }

  public:
    Rect(T top, T right, T bottom, T left)
        : top_(top),
          right_(right),
          bottom_(bottom),
          left_(left)
    {
    }

  public:
    inline const T &top() const { return top_; }
    inline const T &right() const { return right_; }
    inline const T &bottom() const { return bottom_; }
    inline const T &left() const { return left_; }

    const T &operator[](Edge edge) const
    {
      switch (edge)
      {
      case Edge::kTop:
        return top_;
      case Edge::kRight:
        return right_;
      case Edge::kBottom:
        return bottom_;
      case Edge::kLeft:
        return left_;
      default:
        assert(false && "Invalid edge");
        return top(); // Dummy return to avoid compiler warning.
      }
    }

  protected:
    T top_;
    T right_;
    T bottom_;
    T left_;
  };
}
