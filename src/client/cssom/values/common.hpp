#pragma once

namespace client_cssom::values
{
  class CSSFloat
  {
  public:
    CSSFloat() : value(0.0f) {}
    CSSFloat(float v) : value(v) {}

  public:
    inline bool isZero() const { return value == 0.0f; }

  public:
    float value;
  };
}
