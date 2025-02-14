#pragma once

#include <glm/glm.hpp>
#include <skia/include/core/SkColor.h>
#include <crates/bindings.hpp>

namespace client_cssom::types
{
  class Color
  {
  public:
    Color() : color_(0, 0, 0, 255) {}
    Color(uint32_t r, uint32_t g, uint32_t b, uint32_t a = 255) : color_(r, g, b, a) {}
    Color(const std::string &colorStr) : colorStr_(colorStr)
    {
      auto rgba = crates::css2::parsing::parseColor(colorStr);
      color_ = glm::u32vec4(rgba.r(), rgba.g(), rgba.b(), rgba.a());
    }

  public:
    inline uint32_t r() const { return color_.r; }
    inline uint32_t g() const { return color_.g; }
    inline uint32_t b() const { return color_.b; }
    inline uint32_t a() const { return color_.a; }

  public:
    // Output to ostream
    friend std::ostream &operator<<(std::ostream &os, const Color &color)
    {
      os << "rgba(" << color.r() << ", " << color.g() << ", " << color.b() << ", " << color.a() << ")";
      return os;
    }
    // Convert to SkColor
    operator SkColor() const
    {
      return SkColorSetARGB(a(), r(), g(), b());
    }

  private:
    glm::u32vec4 color_;
    std::string colorStr_;
  };
}
