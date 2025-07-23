#include "./image.hpp"
#include <client/cssom/values/computed/image.hpp>
#include <client/cssom/parsers/css_image_parser.hpp>

namespace client_cssom::values::specified
{
  bool Image::parse(const std::string &input)
  {
    // Use the new CSS image parser with proper tokenization
    *this = css_parser::CSSImageParser::parseImage(input);
    return !isNone() || input == "none";
  }

  std::string Image::toCss() const
  {
    if (isGradient())
    {
      const auto &gradient = std::get<Gradient>(*this);
      return gradientToCss(gradient);
    }
    else
    {
      // Delegate to base class for monostate and URL cases
      return generics::GenericImage<Gradient, specified::UrlOrNone>::toCss();
    }
  }

  computed::Image Image::toComputedValue(computed::Context &context) const
  {
    computed::Image computed_img;

    if (isNone())
    {
      computed_img.emplace<std::monostate>();
    }
    else if (isUrl())
    {
      const auto &url_or_none = std::get<UrlOrNone>(*this);
      computed_img.emplace<UrlOrNone>(url_or_none);
    }
    else if (isGradient())
    {
      const auto &gradient = std::get<Gradient>(*this);
      computed::Gradient computed_gradient = gradient.toComputedValue(context);
      computed_img.emplace<computed::Gradient>(computed_gradient);
    }

    return computed_img;
  }

  std::string Image::gradientToCss(const Gradient &gradient) const
  {
    // Enhanced gradient serialization with proper CSS output
    if (holds_alternative<Gradient::LinearGradient>(gradient.gradient_type))
    {
      const auto &linearGrad = std::get<Gradient::LinearGradient>(gradient.gradient_type);
      std::string functionName = gradient.repeating ? "repeating-linear-gradient" : "linear-gradient";

      std::string direction;
      switch (linearGrad.direction)
      {
      case generics::LineDirection::kToRight:
        direction = "to right";
        break;
      case generics::LineDirection::kToLeft:
        direction = "to left";
        break;
      case generics::LineDirection::kToTop:
        direction = "to top";
        break;
      case generics::LineDirection::kToBottom:
        direction = "to bottom";
        break;
      case generics::LineDirection::kToTopLeft:
        direction = "to top left";
        break;
      case generics::LineDirection::kToTopRight:
        direction = "to top right";
        break;
      case generics::LineDirection::kToBottomLeft:
        direction = "to bottom left";
        break;
      case generics::LineDirection::kToBottomRight:
        direction = "to bottom right";
        break;
      }

      // TODO: Add proper color stop serialization
      std::string colorStops = "red, blue"; // Placeholder
      if (!linearGrad.items.empty())
      {
        // Serialize actual color stops when color parsing is implemented
      }

      return functionName + "(" + direction + ", " + colorStops + ")";
    }
    else if (holds_alternative<Gradient::RadialGradient>(gradient.gradient_type))
    {
      const auto &radialGrad = std::get<Gradient::RadialGradient>(gradient.gradient_type);
      std::string functionName = gradient.repeating ? "repeating-radial-gradient" : "radial-gradient";
      std::string shape = (radialGrad.shape == generics::RadialGradientShape::kCircle) ? "circle" : "ellipse";

      std::string size;
      switch (radialGrad.size)
      {
      case generics::RadialGradientSize::kClosestSide:
        size = "closest-side";
        break;
      case generics::RadialGradientSize::kClosestCorner:
        size = "closest-corner";
        break;
      case generics::RadialGradientSize::kFarthestSide:
        size = "farthest-side";
        break;
      case generics::RadialGradientSize::kFarthestCorner:
        size = "farthest-corner";
        break;
      }

      // TODO: Add proper color stop serialization
      std::string colorStops = "red, blue"; // Placeholder
      if (!radialGrad.items.empty())
      {
        // Serialize actual color stops when color parsing is implemented
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

  // Gradient conversion to computed value
  computed::Gradient Gradient::toComputedValue(computed::Context &context) const
  {
    computed::Gradient computed_gradient;
    computed_gradient.repeating = this->repeating;

    // Convert gradient type from specified to computed
    if (std::holds_alternative<LinearGradient>(this->gradient_type))
    {
      const auto &specified_linear = std::get<LinearGradient>(this->gradient_type);
      computed::Gradient::LinearGradient computed_linear;

      // Copy direction (no conversion needed for enum)
      computed_linear.direction = specified_linear.direction;

      // Convert gradient items
      for (const auto &item : specified_linear.items)
      {
        computed::Gradient::LinearGradient::GradientItem computed_item;
        computed_item.type = item.type;

        if (item.type == generics::GenericGradientItemBase::kSimpleColorStop)
        {
          const auto &color_stop = std::get<typename generics::GenericGradientItem<Color, LengthPercentage>::SimpleColorStop>(item.value);
          typename computed::Gradient::LinearGradient::GradientItem::SimpleColorStop computed_stop;
          computed_stop.color = color_stop.color.toComputedValue(context);
          computed_item.value = computed_stop;
        }
        else if (item.type == generics::GenericGradientItemBase::kComplexColorStop)
        {
          const auto &color_stop = std::get<typename generics::GenericGradientItem<Color, LengthPercentage>::ComplexColorStop>(item.value);
          typename computed::Gradient::LinearGradient::GradientItem::ComplexColorStop computed_stop;
          computed_stop.color = color_stop.color.toComputedValue(context);
          computed_stop.length_percentage = color_stop.length_percentage.toComputedValue(context);
          computed_item.value = computed_stop;
        }
        else if (item.type == generics::GenericGradientItemBase::kInterpolationHint)
        {
          const auto &hint = std::get<typename generics::GenericGradientItem<Color, LengthPercentage>::InterpolationHint>(item.value);
          typename computed::Gradient::LinearGradient::GradientItem::InterpolationHint computed_hint;
          computed_hint.length_percentage = hint.length_percentage.toComputedValue(context);
          computed_item.value = computed_hint;
        }

        computed_linear.items.push_back(computed_item);
      }

      computed_gradient.gradient_type = computed_linear;
    }
    else if (std::holds_alternative<RadialGradient>(this->gradient_type))
    {
      const auto &specified_radial = std::get<RadialGradient>(this->gradient_type);
      computed::Gradient::RadialGradient computed_radial;

      // Copy shape and size (no conversion needed for enums)
      computed_radial.shape = specified_radial.shape;
      computed_radial.size = specified_radial.size;

      // Convert gradient items
      for (const auto &item : specified_radial.items)
      {
        computed::Gradient::RadialGradient::GradientItem computed_item;
        computed_item.type = item.type;

        if (item.type == generics::GenericGradientItem<Color, LengthPercentage>::kSimpleColorStop)
        {
          const auto &color_stop = std::get<typename generics::GenericGradientItem<Color, LengthPercentage>::SimpleColorStop>(item.value);
          typename computed::Gradient::RadialGradient::GradientItem::SimpleColorStop computed_stop;
          computed_stop.color = color_stop.color.toComputedValue(context);
          computed_item.value = computed_stop;
        }
        else if (item.type == generics::GenericGradientItem<Color, LengthPercentage>::kComplexColorStop)
        {
          const auto &color_stop = std::get<typename generics::GenericGradientItem<Color, LengthPercentage>::ComplexColorStop>(item.value);
          typename computed::Gradient::RadialGradient::GradientItem::ComplexColorStop computed_stop;
          computed_stop.color = color_stop.color.toComputedValue(context);
          computed_stop.length_percentage = color_stop.length_percentage.toComputedValue(context);
          computed_item.value = computed_stop;
        }
        else if (item.type == generics::GenericGradientItem<Color, LengthPercentage>::kInterpolationHint)
        {
          const auto &hint = std::get<typename generics::GenericGradientItem<Color, LengthPercentage>::InterpolationHint>(item.value);
          typename computed::Gradient::RadialGradient::GradientItem::InterpolationHint computed_hint;
          computed_hint.length_percentage = hint.length_percentage.toComputedValue(context);
          computed_item.value = computed_hint;
        }

        computed_radial.items.push_back(computed_item);
      }

      computed_gradient.gradient_type = computed_radial;
    }

    return computed_gradient;
  }
}
