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

  enum class RadialGradientShape
  {
    kCircle,
    kEllipse
  };

  enum class RadialGradientSize
  {
    kClosestSide,
    kClosestCorner,
    kFarthestSide,
    kFarthestCorner
  };

  class GenericGradientItemBase
  {
  public:
    enum ItemType
    {
      kSimpleColorStop,
      kComplexColorStop,
      kInterpolationHint,
    };

  public:
    virtual ~GenericGradientItemBase() = default;

  public:
    ItemType type;
  };

  template <typename Color, typename LP>
  class GenericGradientItem : public GenericGradientItemBase
  {
  public:
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

  private:
    using ItemValue = std::variant<SimpleColorStop, ComplexColorStop, InterpolationHint>;

  public:
    ItemValue value;
  };

  template <typename L, typename LP, typename Color>
  class GenericGradient
  {
  public:
    class LinearGradient
    {
    public:
      using GradientItem = GenericGradientItem<Color, LP>;

      LineDirection direction = LineDirection::kToRight;
      std::vector<GradientItem> items;
    };

    class RadialGradient
    {
    public:
      using GradientItem = GenericGradientItem<Color, LP>;

      RadialGradientShape shape = RadialGradientShape::kEllipse;
      RadialGradientSize size = RadialGradientSize::kFarthestCorner;
      std::vector<GradientItem> items;
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
