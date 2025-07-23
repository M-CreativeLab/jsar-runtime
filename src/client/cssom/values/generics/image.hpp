#pragma once

#include <variant>
#include <optional>
#include <vector>
#include <client/cssom/style_traits.hpp>

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
  class GenericGradient : public ToCss
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

    // CSS serialization implementation
    std::string toCss() const override
    {
      if (std::holds_alternative<LinearGradient>(gradient_type))
      {
        const auto &linearGrad = std::get<LinearGradient>(gradient_type);
        std::string functionName = repeating ? "repeating-linear-gradient" : "linear-gradient";

        std::string direction;
        switch (linearGrad.direction)
        {
        case LineDirection::kToRight:
          direction = "to right";
          break;
        case LineDirection::kToLeft:
          direction = "to left";
          break;
        case LineDirection::kToTop:
          direction = "to top";
          break;
        case LineDirection::kToBottom:
          direction = "to bottom";
          break;
        case LineDirection::kToTopLeft:
          direction = "to top left";
          break;
        case LineDirection::kToTopRight:
          direction = "to top right";
          break;
        case LineDirection::kToBottomLeft:
          direction = "to bottom left";
          break;
        case LineDirection::kToBottomRight:
          direction = "to bottom right";
          break;
        }

        // Serialize color stops
        std::string colorStops = serializeColorStops(linearGrad.items);
        if (colorStops.empty())
        {
          colorStops = "transparent, transparent"; // Fallback
        }

        return functionName + "(" + direction + ", " + colorStops + ")";
      }
      else if (std::holds_alternative<RadialGradient>(gradient_type))
      {
        const auto &radialGrad = std::get<RadialGradient>(gradient_type);
        std::string functionName = repeating ? "repeating-radial-gradient" : "radial-gradient";
        std::string shape = (radialGrad.shape == RadialGradientShape::kCircle) ? "circle" : "ellipse";

        std::string size;
        switch (radialGrad.size)
        {
        case RadialGradientSize::kClosestSide:
          size = "closest-side";
          break;
        case RadialGradientSize::kClosestCorner:
          size = "closest-corner";
          break;
        case RadialGradientSize::kFarthestSide:
          size = "farthest-side";
          break;
        case RadialGradientSize::kFarthestCorner:
          size = "farthest-corner";
          break;
        }

        // Serialize color stops
        std::string colorStops = serializeColorStops(radialGrad.items);
        if (colorStops.empty())
        {
          colorStops = "transparent, transparent"; // Fallback
        }

        std::string shapeSize = shape;
        if (!size.empty())
        {
          shapeSize += " " + size;
        }

        return functionName + "(" + shapeSize + ", " + colorStops + ")";
      }

      return "none";
    }

  private:
    // Helper method to serialize color stops
    template<typename ItemVector>
    std::string serializeColorStops(const ItemVector& items) const
    {
      std::string colorStops;
      for (size_t i = 0; i < items.size(); ++i)
      {
        if (i > 0)
          colorStops += ", ";
        
        const auto &item = items[i];
        if (item.type == GenericGradientItemBase::kSimpleColorStop)
        {
          const auto &colorStop = std::get<typename GenericGradientItem<Color, LP>::SimpleColorStop>(item.value);
          colorStops += colorStop.color.toCss();
        }
        else if (item.type == GenericGradientItemBase::kComplexColorStop)
        {
          const auto &colorStop = std::get<typename GenericGradientItem<Color, LP>::ComplexColorStop>(item.value);
          colorStops += colorStop.color.toCss() + " " + colorStop.length_percentage.toCss();
        }
        else if (item.type == GenericGradientItemBase::kInterpolationHint)
        {
          const auto &hint = std::get<typename GenericGradientItem<Color, LP>::InterpolationHint>(item.value);
          colorStops += hint.length_percentage.toCss();
        }
      }
      return colorStops;
    }
  };

  template <typename G, typename ImageUrl>
  class GenericImage : public std::variant<std::monostate, ImageUrl, G>, public ToCss
  {
  public:
    // CSS serialization
    std::string toCss() const override
    {
      if (std::holds_alternative<std::monostate>(*this))
      {
        return "none";
      }
      else if (std::holds_alternative<ImageUrl>(*this))
      {
        const auto &url_or_none = std::get<ImageUrl>(*this);
        if (url_or_none.url.has_value())
        {
          return "url(\"" + url_or_none.url.value() + "\")";
        }
        return "none";
      }
      else if (std::holds_alternative<G>(*this))
      {
        // Call the gradient's toCss() method
        const auto &gradient = std::get<G>(*this);
        return gradient.toCss();
      }
      return "none";
    }
  };
}
