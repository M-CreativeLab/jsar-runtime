#include "./image.hpp"
#include <regex>
#include <sstream>
#include <algorithm>
#include <client/cssom/values/computed/image.hpp>

namespace client_cssom::values::specified
{
  // Helper function to trim whitespace
  static std::string trim(const std::string &str)
  {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos)
      return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
  }

  // Helper function to extract function content
  static std::string extractFunctionContent(const std::string &input, const std::string &functionName)
  {
    std::string prefix = functionName + "(";
    if (input.length() < prefix.length() + 1 || 
        input.substr(0, prefix.length()) != prefix || 
        input.back() != ')')
    {
      return "";
    }
    return input.substr(prefix.length(), input.length() - prefix.length() - 1);
  }

  // Helper function to split string by comma (respecting nested parentheses)
  static std::vector<std::string> splitByComma(const std::string &str)
  {
    std::vector<std::string> result;
    std::string current;
    int parentheses = 0;
    
    for (char c : str)
    {
      if (c == '(')
      {
        parentheses++;
        current += c;
      }
      else if (c == ')')
      {
        parentheses--;
        current += c;
      }
      else if (c == ',' && parentheses == 0)
      {
        result.push_back(trim(current));
        current.clear();
      }
      else
      {
        current += c;
      }
    }
    
    if (!current.empty())
    {
      result.push_back(trim(current));
    }
    
    return result;
  }

  bool Image::parseGradient(const std::string &input)
  {
    std::string trimmed = trim(input);
    
    // Check for linear-gradient
    if (trimmed.length() >= 15 && trimmed.substr(0, 15) == "linear-gradient")
    {
      std::string content = extractFunctionContent(trimmed, "linear-gradient");
      if (!content.empty())
      {
        return parseLinearGradient(content, false);
      }
    }
    
    // Check for repeating-linear-gradient
    if (trimmed.length() >= 25 && trimmed.substr(0, 25) == "repeating-linear-gradient")
    {
      std::string content = extractFunctionContent(trimmed, "repeating-linear-gradient");
      if (!content.empty())
      {
        return parseLinearGradient(content, true);
      }
    }
    
    // Check for radial-gradient
    if (trimmed.length() >= 14 && trimmed.substr(0, 14) == "radial-gradient")
    {
      std::string content = extractFunctionContent(trimmed, "radial-gradient");
      if (!content.empty())
      {
        return parseRadialGradient(content, false);
      }
    }
    
    // Check for repeating-radial-gradient
    if (trimmed.length() >= 24 && trimmed.substr(0, 24) == "repeating-radial-gradient")
    {
      std::string content = extractFunctionContent(trimmed, "repeating-radial-gradient");
      if (!content.empty())
      {
        return parseRadialGradient(content, true);
      }
    }
    
    return false;
  }

  bool Image::parseLinearGradient(const std::string &content, bool repeating)
  {
    // Parse linear gradient parameters
    std::vector<std::string> parts = splitByComma(content);
    if (parts.empty())
    {
      return false;
    }
    
    generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::LinearGradient linearGrad;
    linearGrad.direction = generics::LineDirection::kToRight; // Default direction
    
    // Check if first part is a direction
    std::string firstPart = trim(parts[0]);
    bool hasDirection = false;
    
    if (firstPart == "to right")
    {
      linearGrad.direction = generics::LineDirection::kToRight;
      hasDirection = true;
    }
    else if (firstPart == "to left")
    {
      linearGrad.direction = generics::LineDirection::kToLeft;
      hasDirection = true;
    }
    else if (firstPart == "to top")
    {
      linearGrad.direction = generics::LineDirection::kToTop;
      hasDirection = true;
    }
    else if (firstPart == "to bottom")
    {
      linearGrad.direction = generics::LineDirection::kToBottom;
      hasDirection = true;
    }
    else if (firstPart == "to top left")
    {
      linearGrad.direction = generics::LineDirection::kToTopLeft;
      hasDirection = true;
    }
    else if (firstPart == "to top right")
    {
      linearGrad.direction = generics::LineDirection::kToTopRight;
      hasDirection = true;
    }
    else if (firstPart == "to bottom left")
    {
      linearGrad.direction = generics::LineDirection::kToBottomLeft;
      hasDirection = true;
    }
    else if (firstPart == "to bottom right")
    {
      linearGrad.direction = generics::LineDirection::kToBottomRight;
      hasDirection = true;
    }
    // TODO: Add support for angle values (45deg, 1.5708rad, etc.)
    
    // Parse color stops (skip direction if present)
    size_t startIndex = hasDirection ? 1 : 0;
    for (size_t i = startIndex; i < parts.size(); i++)
    {
      std::string colorStop = trim(parts[i]);
      if (!colorStop.empty())
      {
        // For now, create simple color stops
        // TODO: Parse actual color values and optional length/percentage positions
        generics::GenericGradientItem<Color, LengthPercentage> item;
        linearGrad.items.push_back(item);
      }
    }
    
    // Create the gradient
    Gradient gradient(linearGrad);
    gradient.repeating = repeating;
    
    *this = Image();
    this->emplace<Gradient>(gradient);
    return true;
  }

  bool Image::parseRadialGradient(const std::string &content, bool repeating)
  {
    // Parse radial gradient parameters
    std::vector<std::string> parts = splitByComma(content);
    if (parts.empty())
    {
      return false;
    }
    
    generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient radialGrad;
    radialGrad.shape = generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient::Shape::kEllipse; // Default
    radialGrad.size = generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient::Size::kFarthestCorner; // Default
    
    // Parse shape/size if present (simplified parsing)
    std::string firstPart = trim(parts[0]);
    bool hasShapeSize = false;
    
    if (firstPart == "circle")
    {
      radialGrad.shape = generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient::Shape::kCircle;
      hasShapeSize = true;
    }
    else if (firstPart == "ellipse")
    {
      radialGrad.shape = generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient::Shape::kEllipse;
      hasShapeSize = true;
    }
    else if (firstPart.find("closest-side") != std::string::npos)
    {
      radialGrad.size = generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient::Size::kClosestSide;
      hasShapeSize = true;
    }
    else if (firstPart.find("farthest-side") != std::string::npos)
    {
      radialGrad.size = generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient::Size::kFarthestSide;
      hasShapeSize = true;
    }
    else if (firstPart.find("closest-corner") != std::string::npos)
    {
      radialGrad.size = generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient::Size::kClosestCorner;
      hasShapeSize = true;
    }
    else if (firstPart.find("farthest-corner") != std::string::npos)
    {
      radialGrad.size = generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient::Size::kFarthestCorner;
      hasShapeSize = true;
    }
    
    // Parse color stops (skip shape/size if present)
    size_t startIndex = hasShapeSize ? 1 : 0;
    for (size_t i = startIndex; i < parts.size(); i++)
    {
      std::string colorStop = trim(parts[i]);
      if (!colorStop.empty())
      {
        // For now, create simple color stops
        // TODO: Parse actual color values and optional length/percentage positions
        generics::GenericGradientItem<Color, LengthPercentage> item;
        radialGrad.items.push_back(item);
      }
    }
    
    // Create the gradient
    Gradient gradient(radialGrad);
    gradient.repeating = repeating;
    
    *this = Image();
    this->emplace<Gradient>(gradient);
    return true;
  }

  std::string Image::gradientToCss(const Gradient &gradient) const
  {
    // Enhanced gradient serialization
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
      
      // Basic serialization with placeholder colors
      return functionName + "(" + direction + ", red, blue)";
    }
    else if (std::holds_alternative<generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient>(gradient.gradient_type))
    {
      const auto &radialGrad = std::get<generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient>(gradient.gradient_type);
      std::string functionName = gradient.repeating ? "repeating-radial-gradient" : "radial-gradient";
      
      std::string shape = (radialGrad.shape == generics::GenericGradient<NoCalcLength, LengthPercentage, Color>::RadialGradient::Shape::kCircle) ? "circle" : "ellipse";
      
      // Basic serialization with placeholder colors
      return functionName + "(" + shape + ", red, blue)";
    }
    
    return "none";
  }

  // Gradient conversion to computed value
  computed::Gradient Gradient::toComputedValue(computed::Context &context) const
  {
    computed::Gradient computed_gradient;
    computed_gradient.repeating = this->repeating;
    
    // TODO: Convert specified gradient components to computed values
    // This would involve converting lengths, percentages, and colors
    // to their computed equivalents
    
    return computed_gradient;
  }
}