#pragma once

#include <crates/bindings.hpp>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/common.hpp>
#include <client/cssom/values/generics/border.hpp>
#include <client/cssom/values/generics/rect.hpp>
#include <client/cssom/values/computed/color.hpp>
#include <client/cssom/values/computed/length_percentage.hpp>

namespace client_cssom::values::computed
{
  // A computed value for -webkit-text-stroke-width.
  using LineWidth = CSSFloat;

  // A computed value for border-width (and the like).
  class BorderSideWidth : public CSSFloat,
                          public ToLayoutValue<crates::layout2::styles::LengthPercentage>
  {
    using CSSFloat::CSSFloat;

  public:
    crates::layout2::styles::LengthPercentage toLayoutValue() const
    {
      return crates::layout2::styles::LengthPercentage::Length(value);
    }
  };

  // A computed value for the `border-style` property.
  class BorderSideStyle : public generics::GenericBorderStyle<BorderSideStyle>
  {
    using generics::GenericBorderStyle<BorderSideStyle>::GenericBorderStyle;
  };

  // A computed value for the `border-width` property.
  class BorderWidth : public generics::Rect<BorderSideWidth>
  {
    using generics::Rect<BorderSideWidth>::Rect;

  public:
    BorderWidth(generics::Rect<BorderSideWidth> rect)
        : generics::Rect<BorderSideWidth>(rect)
    {
    }
  };

  // A computed value for the `border-color` property.
  class BorderColor : public generics::Rect<Color>
  {
    using generics::Rect<Color>::Rect;

  public:
    BorderColor(generics::Rect<Color> rect)
        : generics::Rect<Color>(rect)
    {
    }
  };

  // A computed value for the `border-style` property.
  class BorderStyle : public generics::Rect<BorderSideStyle>
  {
    using generics::Rect<BorderSideStyle>::Rect;

  public:
    BorderStyle(generics::Rect<BorderSideStyle> rect)
        : generics::Rect<BorderSideStyle>(rect)
    {
    }
  };

  // A computed value for the `border-image-width` property.
  using BorderImageWidth = BorderWidth;

  // A computed value for the `border-*-radius` longhand properties.
  class BorderCornerRadius : public generics::GenericBorderCornerRadius<NonNegativeLengthPercentage>
  {
    using generics::GenericBorderCornerRadius<NonNegativeLengthPercentage>::GenericBorderCornerRadius;

  public:
    static BorderCornerRadius Zero() { return BorderCornerRadius(0.0f); }

  public:
    BorderCornerRadius(float px)
        : GenericBorderCornerRadius(NonNegativeLengthPercentage::Length(px))
    {
    }
  };

  // A computed value for the `border-radius` property.
  class BorderRadius : public generics::GenericBorderRadius<BorderCornerRadius>
  {
    using generics::GenericBorderRadius<BorderCornerRadius>::GenericBorderRadius;

  public:
    static BorderRadius Zero()
    {
      return BorderRadius(BorderCornerRadius::Zero(),
                          BorderCornerRadius::Zero(),
                          BorderCornerRadius::Zero(),
                          BorderCornerRadius::Zero());
    }
  };

}
