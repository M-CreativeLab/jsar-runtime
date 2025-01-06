#include <optional>
#include "./css_style_declaration.hpp"

namespace client_cssom
{
  using namespace std;
  using namespace crates::jsar::layout;
  using namespace crates::jsar::layout::style;

  inline optional<Dimension> GetDimension(const std::string &value)
  {
    if (value == "auto")
    {
      return Dimension::Auto();
    }
    else if (value.ends_with("%"))
    {
      auto percent = std::stof(value.substr(0, value.size() - 1)) / 100.0f;
      return Dimension::Percent(percent);
    }
    else if (value.ends_with("px"))
    {
      auto pixels = std::stof(value.substr(0, value.size() - 2));
      return Dimension::Length(pixels);
    }
    else
    {
      return nullopt;
    }
  }

  inline optional<Overflow> GetOverflow(const std::string &value)
  {
    if (value == "visible")
      return Overflow::kVisible;
    else if (value == "hidden")
      return Overflow::kHidden;
    else if (value == "scroll")
      return Overflow::kScroll;
    else if (value == "clip")
      return Overflow::kClip;
    else
      return nullopt;
  }

  CSSStyleDeclaration::operator LayoutStyle() const
  {
    LayoutStyle layoutStyle;

    // Set display
    if (properties_.find("display") != properties_.end())
    {
      auto displayStr = properties_.at("display").value;
      if (displayStr == "block")
        layoutStyle.display = Display::kBlock;
      else if (displayStr == "none")
        layoutStyle.display = Display::kNone;
    }

    // Set position
    if (properties_.find("position") != properties_.end())
    {
      auto positionStr = properties_.at("position").value;
      if (positionStr == "absolute")
        layoutStyle.position = Position::kAbsolute;
      else if (positionStr == "relative")
        layoutStyle.position = Position::kRelative;
    }

    // Set width
    if (properties_.find("width") != properties_.end())
    {
      auto widthDimension = GetDimension(properties_.at("width").value);
      if (widthDimension.has_value())
        layoutStyle.width = widthDimension.value();
    }

    // Set height
    if (properties_.find("height") != properties_.end())
    {
      auto heightDimension = GetDimension(properties_.at("height").value);
      if (heightDimension.has_value())
        layoutStyle.height = heightDimension.value();
    }

    // Set overflow(x, y)
    if (properties_.find("overflow") != properties_.end())
    {
      auto overflow = GetOverflow(properties_.at("overflow").value);
      if (overflow.has_value())
      {
        layoutStyle.overflowX = overflow.value();
        layoutStyle.overflowY = overflow.value();
      }
    }
    if (properties_.find("overflow-x") != properties_.end())
    {
      auto overflowX = GetOverflow(properties_.at("overflow-x").value);
      if (overflowX.has_value())
        layoutStyle.overflowX = overflowX.value();
    }
    if (properties_.find("overflow-y") != properties_.end())
    {
      auto overflowY = GetOverflow(properties_.at("overflow-y").value);
      if (overflowY.has_value())
        layoutStyle.overflowY = overflowY.value();
    }

    // Set flex-grow
    if (properties_.find("flex-grow") != properties_.end())
      layoutStyle.flexGrow = std::stof(properties_.at("flex-grow").value);

    // Set flex-shrink
    if (properties_.find("flex-shrink") != properties_.end())
      layoutStyle.flexShrink = std::stof(properties_.at("flex-shrink").value);

    // Return the layout style
    return layoutStyle;
  }
}
