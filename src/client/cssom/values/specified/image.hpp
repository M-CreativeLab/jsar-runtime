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
                     specified::Color>
  {
  };

  class Image : public generics::GenericImage<Gradient, specified::UrlOrNone>,
                public Parse,
                public ToCss,
                public ToComputedValue<computed::Image>
  {
    friend class Parse;

  public:
    // Default constructor creates a 'none' image
    Image() = default;
    
    // Copy constructor
    Image(const Image&) = default;
    Image& operator=(const Image&) = default;
    
    // Move constructor
    Image(Image&&) = default;
    Image& operator=(Image&&) = default;

    // Static factory methods
    static Image None()
    {
      Image img;
      img.emplace<std::monostate>();
      return img;
    }

    static Image Url(const std::string& url_str)
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
      
      // Handle url() syntax - simplified parsing
      if (input.length() >= 5 && input.substr(0, 4) == "url(" && input.back() == ')')
      {
        std::string url_content = input.substr(4, input.length() - 5);
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
      
      // TODO: Add gradient parsing support when needed
      
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
        const auto& url_or_none = std::get<UrlOrNone>(*this);
        if (url_or_none.url.has_value())
        {
          return "url(\"" + url_or_none.url.value() + "\")";
        }
        return "none";
      }
      // TODO: Add gradient serialization when needed
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
        const auto& url_or_none = std::get<UrlOrNone>(*this);
        computed_img.emplace<UrlOrNone>(url_or_none);
      }
      else if (isGradient())
      {
        // TODO: Convert gradient to computed gradient when needed
        const auto& gradient = std::get<Gradient>(*this);
        computed::Gradient computed_gradient;
        // For now, just create an empty gradient
        computed_img.emplace<computed::Gradient>(computed_gradient);
      }
      
      return computed_img;
    }
  };
}
