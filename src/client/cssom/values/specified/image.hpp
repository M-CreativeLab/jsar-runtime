#pragma once

#include <variant>
#include <string>
#include <client/cssom/values/generics/image.hpp>
#include <client/cssom/style_traits.hpp>

#include "./length.hpp"
#include "./color.hpp"
#include "./url.hpp"

// Forward declaration for computed types
namespace client_cssom::values::computed
{
  class Image;
  class Context;
}

namespace client_cssom::values::specified
{
  class Gradient : public generics::GenericGradient<
                     specified::NoCalcLength,
                     specified::LengthPercentage,
                     specified::Color>,
                   public ToComputedValue<computed::Gradient>
  {
  public:
    // Convert to computed value
    computed::Gradient toComputedValue(computed::Context &context) const;
  };

  class Image : public generics::GenericImage<Gradient, specified::UrlOrNone>,
                public Parse,
                public ToCss,
                public ToComputedValue<computed::Image>
  {
    friend class Parse;

  private:
    // Helper methods for parsing gradients
    bool parseGradient(const std::string &input);
    bool parseLinearGradient(const std::string &content, bool repeating = false);
    bool parseRadialGradient(const std::string &content, bool repeating = false);
    
    // Helper method for gradient CSS serialization
    std::string gradientToCss(const Gradient &gradient) const;

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
      UrlOrNone url_or_none;
      url_or_none.url = url_str;
      img.emplace<UrlOrNone>(url_or_none);
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

    // Parse implementation
    bool parse(const std::string &input) override
    {
      // Handle 'none' value
      if (input == "none")
      {
        *this = None();
        return true;
      }

      // Handle url() syntax - improved parsing
      if (input.length() >= 5 && input.substr(0, 4) == "url(" && input.back() == ')')
      {
        std::string url_content = input.substr(4, input.length() - 5);
        // Trim whitespace
        size_t start = url_content.find_first_not_of(" \t\n\r");
        size_t end = url_content.find_last_not_of(" \t\n\r");
        if (start != std::string::npos)
        {
          url_content = url_content.substr(start, end - start + 1);
        }
        
        // Remove quotes if present
        if (url_content.length() >= 2 &&
            ((url_content.front() == '"' && url_content.back() == '"') ||
             (url_content.front() == '\'' && url_content.back() == '\'')))
        {
          url_content = url_content.substr(1, url_content.length() - 2);
        }
        *this = Url(url_content);
        return true;
      }

      // Handle gradient functions
      if (parseGradient(input))
      {
        return true;
      }

      // Default to none for unrecognized values
      *this = None();
      return false;
    }

    // CSS serialization
    std::string toCss() const override
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

    // Convert to computed value
    computed::Image toComputedValue(computed::Context &context) const override
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
  };
}
