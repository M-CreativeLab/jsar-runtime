#pragma once

#include <variant>
#include <string>
#include <client/cssom/values/generics/image.hpp>

#include "./length.hpp"
#include "./color.hpp"
#include "./url.hpp"

namespace client_cssom::values::specified
{
  class Gradient : public generics::GenericGradient<
                     specified::NoCalcLength,
                     specified::LengthPercentage,
                     specified::Color>
  {
  };

  class Image : public generics::GenericImage<Gradient, specified::UrlOrNone>
  {
  };
}
