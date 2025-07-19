#include "./image.hpp"
#include <client/cssom/values/computed/image.hpp>
#include <client/cssom/css_image_parser.hpp>

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
    if (isNone())
    {
      return "none";
    }
    else if (isUrl())
    {
      const auto &url_or_none = std::get<UrlOrNone>(*this);
      if (url_or_none.url.has_value())
      {
        return "url(\"" + url_or_none.url.value() + "\")";
      }
      return "none";
    }
    else if (isGradient())
    {
      const auto &gradient = std::get<Gradient>(*this);
      return gradientToCss(gradient);
    }
    return "none";
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
    if (std::holds_alternative<generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::LinearGradient>(gradient.gradient_type))
    {
      const auto &linearGrad = std::get<generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::LinearGradient>(gradient.gradient_type);
      std::string functionName = gradient.repeating ? "repeating-linear-gradient" : "linear-gradient";
      
      std::string direction;
      switch (linearGrad.direction)
      {
        case generics::LineDirection::kToRight: direction = "to right"; break;
        case generics::LineDirection::kToLeft: direction = "to left"; break;
        case generics::LineDirection::kToTop: direction = "to top"; break;
        case generics::LineDirection::kToBottom: direction = "to bottom"; break;
        case generics::LineDirection::kToTopLeft: direction = "to top left"; break;
        case generics::LineDirection::kToTopRight: direction = "to top right"; break;
        case generics::LineDirection::kToBottomLeft: direction = "to bottom left"; break;
        case generics::LineDirection::kToBottomRight: direction = "to bottom right"; break;
      }
      
      // TODO: Add proper color stop serialization
      std::string colorStops = "red, blue"; // Placeholder
      if (!linearGrad.items.empty())
      {
        // Serialize actual color stops when color parsing is implemented
      }
      
      return functionName + "(" + direction + ", " + colorStops + ")";
    }
    else if (std::holds_alternative<generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient>(gradient.gradient_type))
    {
      const auto &radialGrad = std::get<generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient>(gradient.gradient_type);
      std::string functionName = gradient.repeating ? "repeating-radial-gradient" : "radial-gradient";
      
      std::string shape = (radialGrad.shape == generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient::Shape::kCircle) ? "circle" : "ellipse";
      
      std::string size;
      switch (radialGrad.size)
      {
        case generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient::Size::kClosestSide: 
          size = "closest-side"; break;
        case generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient::Size::kClosestCorner: 
          size = "closest-corner"; break;
        case generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient::Size::kFarthestSide: 
          size = "farthest-side"; break;
        case generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient::Size::kFarthestCorner: 
          size = "farthest-corner"; break;
      }
      
      // TODO: Add proper color stop serialization
      std::string colorStops = "red, blue"; // Placeholder
      if (!radialGrad.items.empty())
      {
        // Serialize actual color stops when color parsing is implemented
      }
      
      std::string shapeSize = shape;
      if (!size.empty()) {
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
    
    // Copy the gradient type
    computed_gradient.gradient_type = this->gradient_type;
    
    // TODO: Convert specified gradient components to computed values
    // This would involve converting lengths, percentages, and colors
    // to their computed equivalents using the context
    
    return computed_gradient;
  }
}