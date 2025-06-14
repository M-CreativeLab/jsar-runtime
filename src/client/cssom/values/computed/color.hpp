#pragma once

#include <skia/include/core/SkColor.h>
#include <client/cssom/values/generics/color.hpp>
#include <client/cssom/values/computed/percentage.hpp>

namespace client_cssom::values::computed
{
  // A computed value for `<color>`.
  class Color : public generics::GenericColor<Color, Percentage>
  {
    using generics::GenericColor<Color, Percentage>::GenericColor;

  public:
    static Color Transparent()
    {
      return Absolute(glm::vec4(0, 0, 0, 0));
    }
    static Color Black()
    {
      return Absolute(glm::vec4(0, 0, 0, 255));
    }
    static Color White()
    {
      return Absolute(glm::vec4(255, 255, 255, 255));
    }

  public:
    // Resolve the color to an absolute color.
    SkColor resolveToAbsoluteColor(SkColor current_color = SK_ColorTRANSPARENT) const
    {
      if (isAbsolute())
        return getAbsoluteColor();
      else if (isCurrentColor())
        return current_color;
      else
        return SK_ColorTRANSPARENT; // Default to transparent for unknown
    }
  };
}
