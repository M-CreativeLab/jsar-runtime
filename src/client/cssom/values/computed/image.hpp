#pragma once

#include <client/cssom/values/generics/image.hpp>
#include <client/cssom/values/computed/color.hpp>
#include <client/cssom/values/computed/length.hpp>
#include <client/cssom/values/computed/length_percentage.hpp>
#include <client/cssom/values/computed/url.hpp>

namespace client_cssom::values::computed
{
  class Gradient : public generics::GenericGradient<
                     computed::Length,
                     computed::LengthPercentage,
                     computed::Color>
  {
  };

  class Image : public generics::GenericImage<Gradient, UrlOrNone>
  {
  public:
    static Image None()
    {
      return Image();
    }

  public:
    // Default constructor creates a 'none' image
    Image() = default;

    // Copy constructor
    Image(const Image &) = default;
    Image &operator=(const Image &) = default;

    // Move constructor
    Image(Image &&) = default;
    Image &operator=(Image &&) = default;

    // Check if image is none/empty
    bool isNone() const
    {
      return std::holds_alternative<std::monostate>(*this);
    }

    // Check if image is a URL
    bool isUrl() const
    {
      return std::holds_alternative<UrlOrNone>(*this);
    }

    // Check if image is a gradient
    bool isGradient() const
    {
      return std::holds_alternative<Gradient>(*this);
    }

    // Get URL if it's a URL, otherwise return empty string
    std::string getUrl() const;
  };
}
