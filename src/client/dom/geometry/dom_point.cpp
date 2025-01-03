#include "./dom_point.hpp"

namespace dom::geometry
{
  DOMPoint DOMPointReadOnly::matrixTransform(const DOMMatrixReadOnly &matrix)
  {
    if (matrix.is2D())
    {
      return DOMPoint(
          x() * matrix.a() + y() * matrix.c() + matrix.e(),
          x() * matrix.b() + y() * matrix.d() + matrix.f());
    }
    else
    {
      return DOMPoint(
          x() * matrix.m11() + y() * matrix.m21() + z() * matrix.m31() + w() * matrix.m41(),
          x() * matrix.m12() + y() * matrix.m22() + z() * matrix.m32() + w() * matrix.m42(),
          x() * matrix.m13() + y() * matrix.m23() + z() * matrix.m33() + w() * matrix.m43(),
          x() * matrix.m14() + y() * matrix.m24() + z() * matrix.m34() + w() * matrix.m44());
    }
  }
}
