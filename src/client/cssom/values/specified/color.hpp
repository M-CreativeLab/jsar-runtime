#pragma once

#include <cstdint>
#include <variant>
#include <glm/glm.hpp>
#include <crates/bindings.hpp>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/computed/color.hpp>

namespace client_cssom::values::specified
{
  class Color : public Parse,
                public ToCss,
                public ToComputedValue<computed::Color>
  {
    friend class Parse;

  private:
    enum Tag : uint8_t
    {
      kCurrentColor,
      kAbsolute,
      kColorFunction,
      kColorMix,
      kLightDark,
    };

    struct AbsoluteColorVariant
    {
      uint32_t r;
      uint32_t g;
      uint32_t b;
      uint32_t a;

      operator glm::vec4() const { return glm::vec4(r, g, b, a); }
    };
    using ValueVariant = std::variant<std::monostate, AbsoluteColorVariant>;

  private:
    bool parse(const std::string &input) override
    {
      if (input == "currentcolor")
      {
        tag_ = Tag::kCurrentColor;
        value_ = std::monostate();
      }
      else
      {
        using namespace crates::css2;

        // TODO(yorkie): support color functions, mix, light/dark.
        auto color = parsing::parseColor(input);
        tag_ = Tag::kAbsolute;
        value_ = AbsoluteColorVariant{
            static_cast<uint32_t>(color.r()),
            static_cast<uint32_t>(color.g()),
            static_cast<uint32_t>(color.b()),
            static_cast<uint32_t>(color.a())};
      }
      return true;
    }

  public:
    inline bool isCurrentColor() const { return tag_ == Tag::kCurrentColor; }
    inline bool isAbsoluteColor() const { return tag_ == Tag::kAbsolute; }

    std::string toCss() const override
    {
      if (isCurrentColor())
        return "currentcolor";
      else if (isAbsoluteColor())
      {
        AbsoluteColorVariant absolute_color = std::get<AbsoluteColorVariant>(value_);
        return "rgba(" + std::to_string(absolute_color.r) + ", " +
               std::to_string(absolute_color.g) + ", " +
               std::to_string(absolute_color.b) + ", " +
               std::to_string(absolute_color.a) + ")";
      }
      return "";
    }
    computed::Color toComputedValue(computed::Context &context) const override
    {
      if (isCurrentColor())
      {
        return computed::Color::CurrentColor();
      }
      else if (isAbsoluteColor())
      {
        AbsoluteColorVariant absolute_color = std::get<AbsoluteColorVariant>(value_);
        return computed::Color::Absolute(absolute_color);
      }
      return computed::Color::Transparent();
    }

  private:
    Tag tag_;
    ValueVariant value_;
  };
}
