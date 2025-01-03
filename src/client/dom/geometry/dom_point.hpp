#pragma once

#include "./dom_matrix.hpp"

namespace dom::geometry
{
  struct DOMPointInit
  {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float w = 1.0f;
  };

  class DOMPoint;
  class DOMPointReadOnly
  {
  public:
    inline static DOMPointReadOnly FromPoint(const DOMPointInit init)
    {
      return DOMPointReadOnly(init.x, init.y, init.z, init.w);
    }

  public:
    DOMPointReadOnly(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f)
        : x_(x), y_(y), z_(z), w_(w)
    {
    }

  public:
    float x() const { return x_; }
    float y() const { return y_; }
    float z() const { return z_; }
    float w() const { return w_; }

  public:
    DOMPoint matrixTransform(const DOMMatrixReadOnly &matrix);

  protected:
    float x_ = 0.0f;
    float y_ = 0.0f;
    float z_ = 0.0f;
    float w_ = 1.0f;
  };

  class DOMPoint : public DOMPointReadOnly
  {
  public:
    inline static DOMPoint FromPoint(const DOMPointInit init)
    {
      return DOMPoint(init.x, init.y, init.z, init.w);
    }

  public:
    using DOMPointReadOnly::DOMPointReadOnly;

  public:
    float &x() { return x_; }
    float &y() { return y_; }
    float &z() { return z_; }
    float &w() { return w_; }
  };
}
