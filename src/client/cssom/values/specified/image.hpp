#pragma once

#include <variant>
#include <string>
#include <client/cssom/values/generics/image.hpp>
#include <client/cssom/values/computed/image.hpp>
#include <client/cssom/style_traits.hpp>

#include "./length.hpp"
#include "./color.hpp"
#include "./url.hpp"

namespace endor
{
  namespace client_cssom::values::specified
  {
    using GradientItem = generics::GenericGradientItem<specified::Color, specified::LengthPercentage>;
    using GradientBase = generics::GenericGradient<specified::NoCalcLength,
                                                   specified::LengthPercentage,
                                                   specified::Color>;

    class Gradient : public GradientBase,
                     public ToComputedValue<computed::Gradient>
    {
      using GradientBase::GenericGradient;

    public:
      // Convert to computed value
      computed::Gradient toComputedValue(computed::Context &context) const;
    };

    class Image : public generics::GenericImage<Gradient, specified::UrlOrNone>,
                  public Parse,
                  public ToComputedValue<computed::Image>
    {
      friend class Parse;

    public:
      // Default constructor creates a 'none' image
      Image() = default;

      // Copy constructor
      Image(const Image &) = default;
      Image &operator=(const Image &) = default;

      // Move constructor
      Image(Image &&) = default;
      Image &operator=(Image &&) = default;

      // Static factory methods
      static Image None()
      {
        Image img;
        img.emplace<std::monostate>();
        return img;
      }

      static Image Url(const std::string &url_str)
      {
        Image img;
        img.emplace<UrlOrNone>(UrlOrNone::Url(url_str));
        return img;
      }

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

      // Parse implementation using proper CSS tokenizer and parser
      bool parse(const std::string &input) override;

      // CSS serialization (override to provide gradient-specific behavior)
      std::string toCss() const override;

      // Convert to computed value
      computed::Image toComputedValue(computed::Context &context) const override;

    private:
      // Helper method for gradient CSS serialization
      std::string gradientToCss(const Gradient &gradient) const;
    };
  }
} // namespace endor
