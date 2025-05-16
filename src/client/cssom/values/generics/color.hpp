#pragma once

#include <variant>
#include <glm/glm.hpp>
#include <skia/include/core/SkColor.h>
#include <client/cssom/style_traits.hpp>

namespace client_cssom::values::generics
{
  template <typename T, typename Percentage>
  class GenericColor : public ToCss
  {
  private:
    enum Tag : uint8_t
    {
      kAbsolute,
      kColorFunction,
      kCurrentColor,
      // TODO: Color mix
    };

    struct AbsoluteColorVariant
    {
      Tag tag;
      glm::u32vec4 rgba;

      static AbsoluteColorVariant From(const glm::vec4 &rgba)
      {
        return AbsoluteColorVariant{kAbsolute, rgba};
      }
      static AbsoluteColorVariant From(const SkColor color)
      {
        return AbsoluteColorVariant{kAbsolute, glm::u32vec4(SkColorGetR(color),
                                                            SkColorGetG(color),
                                                            SkColorGetB(color),
                                                            SkColorGetA(color))};
      }

      inline uint32_t r() const { return rgba.r; }
      inline uint32_t g() const { return rgba.g; }
      inline uint32_t b() const { return rgba.b; }
      inline uint32_t a() const { return rgba.a; }
    };

    struct ColorFunctionVariant
    {
      Tag tag;
      void *color_function_ptr;
    };

    struct CurrentColorVariant
    {
      Tag tag;
    };

    using Variant = std::variant<AbsoluteColorVariant,
                                 ColorFunctionVariant,
                                 CurrentColorVariant>;

  public:
    // Returns an absolute color from the given RGBA values.
    static inline T Absolute(const glm::vec4 &rgba)
    {
      return T(AbsoluteColorVariant::From(rgba));
    }
    // Returns an absolute color from the given RGBA values.
    static inline T Absolute(float r, float g, float b, float a)
    {
      return T(Absolute(glm::vec4(r, g, b, a)));
    }
    // Returns a color that represents current color.
    static inline T CurrentColor()
    {
      return T(CurrentColorVariant{Tag::kCurrentColor});
    }

  protected:
    GenericColor(const SkColor color = SK_ColorTRANSPARENT)
        : variant_(AbsoluteColorVariant::From(color))
    {
    }
    GenericColor(const AbsoluteColorVariant &color)
        : variant_(color)
    {
    }
    GenericColor(const ColorFunctionVariant &color_function)
        : variant_(color_function)
    {
    }
    GenericColor(const CurrentColorVariant &current_color)
        : variant_(current_color)
    {
    }

  public:
    inline bool isAbsolute() const { return std::holds_alternative<AbsoluteColorVariant>(variant_); }
    inline bool isColorFunction() const { return std::holds_alternative<ColorFunctionVariant>(variant_); }
    inline bool isCurrentColor() const { return std::holds_alternative<CurrentColorVariant>(variant_); }

    inline SkColor getAbsoluteColor() const
    {
      if (isAbsolute())
      {
        const auto &color = std::get<AbsoluteColorVariant>(variant_);
        return SkColorSetARGB(color.a(), color.r(), color.g(), color.b());
      }
      return SK_ColorTRANSPARENT; // Default to transparent for unknown
    }

    std::string toCss() const override
    {
      std::stringstream ss;
      if (isAbsolute())
      {
        const auto &color = std::get<AbsoluteColorVariant>(variant_);
        ss << "rgba(" << color.r() << ", " << color.g() << ", " << color.b() << ", " << color.a() << ")";
      }
      else if (isCurrentColor())
      {
        ss << "currentColor";
      }
      else
      {
        ss << "unknown";
      }
      return ss.str();
    }

    // Output to ostream
    friend std::ostream &operator<<(std::ostream &os, const GenericColor &color)
    {
      os << color.toCss();
      return os;
    }

  private:
    Variant variant_;
  };
}
