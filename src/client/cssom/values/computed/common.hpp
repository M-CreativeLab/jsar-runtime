#pragma once

#include <client/cssom/media_queries.hpp>
#include <client/cssom/values/common.hpp>
#include <client/cssom/values/generics/common.hpp>

namespace client_cssom::values::computed
{
  using NonNegativeNumber = generics::NonNegative<CSSFloat>;

  // A computed value for `min-width`, `min-height`, `width` or `height` property.
  class CSSPixelLength
  {
  public:
    CSSPixelLength(float px)
        : value(px)
    {
    }

    bool operator==(const CSSPixelLength &other) const { return value == other.value; }
    bool operator==(float other) const { return value == other; }
    bool operator<(const CSSPixelLength &other) const { return value < other.value; }
    bool operator<(float other) const { return value < other; }
    bool operator>(const CSSPixelLength &other) const { return value > other.value; }
    bool operator>(float other) const { return value > other; }

  public:
    inline float px() const { return value; }

    // Returns the absolute value of the CSSPixelLength.
    inline CSSPixelLength abs() const
    {
      return CSSPixelLength(std::abs(value));
    }

    inline CSSPixelLength min(const CSSPixelLength &other) const
    {
      return CSSPixelLength(std::min(value, other.value));
    }
    inline CSSPixelLength max(const CSSPixelLength &other) const
    {
      return CSSPixelLength(std::max(value, other.value));
    }

  public:
    float value;
  };

  // Context for computed value conversion.
  class Context
  {
  public:
    Context() {}

  public:
    float fontSize() const { return device_.rootFontSize(); }
    int fontWeight() const { return 400; }
    float lineHeight() const { return device_.rootLineHeight(); }
    glm::uvec4 baseViewport() const { return glm::uvec4(1920, 1080, 0, 0); }

  private:
    Device device_;
    // TODO(yorkie): store the inherited style and default style.

    bool in_media_query_ = false;
    bool in_container_query_ = false;
  };
}
