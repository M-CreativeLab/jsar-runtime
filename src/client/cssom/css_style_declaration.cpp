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
    if (hasProperty("display"))
    {
      auto displayStr = getPropertyValue("display");
      if (displayStr == "block")
        layoutStyle.display = Display::kBlock;
      else if (displayStr == "none")
        layoutStyle.display = Display::kNone;
    }

    // Set position
    if (hasProperty("position"))
    {
      auto positionStr = getPropertyValue("position");
      if (positionStr == "absolute")
        layoutStyle.position = Position::kAbsolute;
      else if (positionStr == "relative")
        layoutStyle.position = Position::kRelative;
    }

    // Set width
    if (hasProperty("width"))
    {
      auto widthDimension = GetDimension(getPropertyValue("width"));
      if (widthDimension.has_value())
        layoutStyle.width = widthDimension.value();
    }

    // Set height
    if (hasProperty("height"))
    {
      auto heightDimension = GetDimension(getPropertyValue("height"));
      if (heightDimension.has_value())
        layoutStyle.height = heightDimension.value();
    }

    // Set overflow(x, y)
    if (hasProperty("overflow"))
    {
      auto overflow = GetOverflow(getPropertyValue("overflow"));
      if (overflow.has_value())
      {
        layoutStyle.overflowX = overflow.value();
        layoutStyle.overflowY = overflow.value();
      }
    }
    if (hasProperty("overflow-x"))
    {
      auto overflowX = GetOverflow(getPropertyValue("overflow-x"));
      if (overflowX.has_value())
        layoutStyle.overflowX = overflowX.value();
    }
    if (hasProperty("overflow-y"))
    {
      auto overflowY = GetOverflow(getPropertyValue("overflow-y"));
      if (overflowY.has_value())
        layoutStyle.overflowY = overflowY.value();
    }

    // Set flex-grow
    if (hasProperty("flex-grow"))
      layoutStyle.flexGrow = std::stof(getPropertyValue("flex-grow"));

    // Set flex-shrink
    if (hasProperty("flex-shrink"))
      layoutStyle.flexShrink = std::stof(getPropertyValue("flex-shrink"));

    // Return the layout style
    return layoutStyle;
  }
}
