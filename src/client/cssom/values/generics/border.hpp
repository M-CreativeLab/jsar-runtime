#pragma once

#include <string>
#include "./rect.hpp"

namespace client_cssom::values::generics
{
  using BorderEdge = Edge;

  enum class BorderCorner
  {
    kTopLeft,
    kTopRight,
    kBottomRight,
    kBottomLeft
  };

  inline std::string to_string(const BorderCorner &corner)
  {
    switch (corner)
    {
    case BorderCorner::kTopLeft:
      return "border-top-left-radius";
    case BorderCorner::kTopRight:
      return "border-top-right-radius";
    case BorderCorner::kBottomRight:
      return "border-bottom-right-radius";
    case BorderCorner::kBottomLeft:
      return "border-bottom-left-radius";
    default:
      return "";
    }
  }

  template <typename T>
  class GenericBorderStyle : public ToCss
  {
  protected:
    enum Tag
    {
      kHidden,
      kNone,
      kInset,
      kGroove,
      kOutset,
      kRidge,
      kDotted,
      kDashed,
      kSolid,
      kDouble,
    };

  public:
    static T Hidden()
    {
      return T(kHidden);
    }
    static T None()
    {
      return T(kNone);
    }
    static T Inset()
    {
      return T(kInset);
    }
    static T Groove()
    {
      return T(kGroove);
    }
    static T Outset()
    {
      return T(kOutset);
    }
    static T Ridge()
    {
      return T(kRidge);
    }
    static T Dotted()
    {
      return T(kDotted);
    }
    static T Dashed()
    {
      return T(kDashed);
    }
    static T Solid()
    {
      return T(kSolid);
    }
    static T Double()
    {
      return T(kDouble);
    }

  public:
    GenericBorderStyle()
        : tag_(kNone)
    {
    }

  protected:
    GenericBorderStyle(Tag tag)
        : tag_(tag)
    {
    }

  public:
    inline bool isNoneOrHidden() const
    {
      return tag_ == kNone || tag_ == kHidden;
    }
    inline bool isInset() const
    {
      return tag_ == kInset;
    }
    inline bool isGroove() const
    {
      return tag_ == kGroove;
    }
    inline bool isOutset() const
    {
      return tag_ == kOutset;
    }
    inline bool isRidge() const
    {
      return tag_ == kRidge;
    }
    inline bool isDotted() const
    {
      return tag_ == kDotted;
    }
    inline bool isDashed() const
    {
      return tag_ == kDashed;
    }
    inline bool isSolid() const
    {
      return tag_ == kSolid;
    }
    inline bool isDouble() const
    {
      return tag_ == kDouble;
    }

    std::string toCss() const override
    {
      switch (tag_)
      {
      case kHidden:
        return "hidden";
      case kNone:
        return "none";
      case kInset:
        return "inset";
      case kGroove:
        return "groove";
      case kOutset:
        return "outset";
      case kRidge:
        return "ridge";
      case kDotted:
        return "dotted";
      case kDashed:
        return "dashed";
      case kSolid:
        return "solid";
      case kDouble:
        return "double";
      default:
        assert(false && "Invalid border style");
        return "";
      }
    }

  protected:
    Tag tag_;
  };

  template <typename L>
  class GenericBorderCornerRadius
  {
  public:
    GenericBorderCornerRadius() = default;
    GenericBorderCornerRadius(const L &value)
        : value_(value)
    {
    }

  public:
    const L &lengthPercentage() const
    {
      return value_;
    }

  protected:
    L value_;
  };

  template <typename C>
  class GenericBorderRadius
  {
  public:
    GenericBorderRadius() = default;
    GenericBorderRadius(const C &top_left,
                        const C &top_right,
                        const C &bottom_right,
                        const C &bottom_left)
        : top_left_(top_left)
        , top_right_(top_right)
        , bottom_right_(bottom_right)
        , bottom_left_(bottom_left)
    {
    }

  public:
    const C &topLeft() const
    {
      return top_left_;
    }
    C &topLeft()
    {
      return top_left_;
    }
    const C &topRight() const
    {
      return top_right_;
    }
    C &topRight()
    {
      return top_right_;
    }
    const C &bottomLeft() const
    {
      return bottom_left_;
    }
    C &bottomLeft()
    {
      return bottom_left_;
    }
    const C &bottomRight() const
    {
      return bottom_right_;
    }
    C &bottomRight()
    {
      return bottom_right_;
    }

    // Support for accessing corners using BorderCorner enum
    const C &operator[](const BorderCorner &corner) const
    {
      switch (corner)
      {
      case BorderCorner::kTopLeft:
        return top_left_;
      case BorderCorner::kTopRight:
        return top_right_;
      case BorderCorner::kBottomLeft:
        return bottom_left_;
      case BorderCorner::kBottomRight:
        return bottom_right_;
      default:
        throw std::out_of_range("Invalid border corner");
      }
    }

  protected:
    C top_left_;
    C top_right_;
    C bottom_left_;
    C bottom_right_;
  };
}
