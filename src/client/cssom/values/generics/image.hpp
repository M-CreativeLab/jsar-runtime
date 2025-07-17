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
  private:
    class LinearGradient
    {
    public:
      LineDirection direction;
      std::vector<GenericGradientItem<Color, LP>> items;
    };

    // TODO(yorkie): add radial gradient when needed.
    // TODO(yorkie): add conic gradient when needed.

  public:
    bool repeating;
  };

  template <typename G, typename ImageUrl>
  class GenericImage : public std::variant<std::monostate, ImageUrl, G>
  {
  };
}
