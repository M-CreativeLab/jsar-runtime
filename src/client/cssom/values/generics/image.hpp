#pragma once

#include <variant>
#include <optional>
#include <vector>

namespace client_cssom::values::generics
{
  enum class LineDirection
  {
    kToLeft,
    kToRight,
    kToTop,
    kToBottom,
    kToTopLeft,
    kToTopRight,
    kToBottomLeft,
    kToBottomRight
  };

  template <typename Color, typename LP>
  class GenericGradientItem
  {
    enum ItemType
    {
      kSimpleColorStop,
      kComplexColorStop,
      kInterpolationHint,
    };

    struct SimpleColorStop
    {
      Color color;
    };
    struct ComplexColorStop
    {
      Color color;
      LP length_percentage; // Length or percentage for the stop position.
    };
    struct InterpolationHint
    {
      LP length_percentage; // Length or percentage for the hint position.
    };
    using ItemValue = std::variant<SimpleColorStop, ComplexColorStop, InterpolationHint>;

  public:
    ItemType type;
    ItemValue value;
  };

  template <typename L, typename LP, typename Color>
  class GenericGradient
  {
  public:
    class LinearGradient
    {
    public:
      LineDirection direction = LineDirection::kToRight;
      std::vector<GenericGradientItem<Color, LP>> items;
    };

    class RadialGradient
    {
    public:
      enum Shape
      {
        kCircle,
        kEllipse
      };
      enum Size
      {
        kClosestSide,
        kClosestCorner,
        kFarthestSide,
        kFarthestCorner
      };

      Shape shape = Shape::kEllipse;
      Size size = Size::kFarthestCorner;
      std::vector<GenericGradientItem<Color, LP>> items;
    };

    // TODO(yorkie): add conic gradient when needed.

    using GradientType = std::variant<LinearGradient, RadialGradient>;

  public:
    bool repeating = false;
    GradientType gradient_type;

    // Constructor for linear gradient
    GenericGradient(const LinearGradient &linear)
        : gradient_type(linear)
    {
    }

    // Constructor for radial gradient
    GenericGradient(const RadialGradient &radial)
        : gradient_type(radial)
    {
    }

    // Default constructor (creates linear gradient)
    GenericGradient()
        : gradient_type(LinearGradient{})
    {
    }
  };

  template <typename G, typename ImageUrl>
  class GenericImage : public std::variant<std::monostate, ImageUrl, G>
  {
  };
}
