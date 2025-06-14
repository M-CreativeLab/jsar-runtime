#include "./path2d.hpp"

namespace canvas
{
  inline float radToDeg(float rad)
  {
    return rad * 180 / M_PI;
  }

  inline float degToRad(int deg)
  {
    return deg * M_PI / 180;
  }

  inline bool almostEqual(float a, float b, float epsilon = 0.00001)
  {
    return fabs(a - b) < epsilon;
  }

  void ellipseHelper(std::shared_ptr<SkPath> skPath,
                     float x,
                     float y,
                     float radiusX,
                     float radiusY,
                     float startAngle,
                     float endAngle)
  {
    auto sweepDegrees = radToDeg(endAngle - startAngle);
    auto startDegrees = radToDeg(startAngle);

    auto oval = SkRect::MakeLTRB(x - radiusX, y - radiusY, x + radiusX, y + radiusY);
    if (almostEqual(fabs(sweepDegrees), 360))
    {
      auto halfSweep = sweepDegrees / 2;
      skPath->arcTo(oval, startDegrees, halfSweep, false);
      skPath->arcTo(oval, startDegrees + halfSweep, halfSweep, false);
    }
    else
    {
      skPath->arcTo(oval, startDegrees, sweepDegrees, false);
    }
  }

  void Path2D::ApplyArcToTangent(std::shared_ptr<SkPath> skPath, SkScalar x1, SkScalar y1, SkScalar x2, SkScalar y2, SkScalar radius)
  {
    skPath->arcTo(x1, y1, x2, y2, radius);
  }

  void Path2D::Ellipse(std::shared_ptr<SkPath> skPath,
                       float x,
                       float y,
                       float radiusX,
                       float radiusY,
                       float rotation,
                       float startAngle,
                       float endAngle,
                       bool ccw)
  {
    if (radiusX < 0 || radiusY < 0)
      throw std::runtime_error("radii cannot be negative");

    // based off of CanonicalizeAngle in Chrome.
    auto tao = 2 * M_PI;
    auto newStartAngle = fmod(startAngle, tao);
    if (newStartAngle < 0)
      newStartAngle += tao;
    auto delta = newStartAngle - startAngle;
    startAngle = newStartAngle;
    endAngle += delta;

    // Based off of AdjustEndAngle in Chrome.
    if (!ccw && (endAngle - startAngle) >= tao)
      endAngle = startAngle + tao;
    else if (ccw && (startAngle - endAngle) >= tao)
      endAngle = startAngle - tao;
    else if (!ccw && startAngle > endAngle)
      endAngle = startAngle + (tao - fmod(startAngle - endAngle, tao));
    else if (ccw && startAngle < endAngle)
      endAngle = startAngle - (tao - fmod(endAngle - startAngle, tao));

    if (!rotation)
    {
      ellipseHelper(skPath, x, y, radiusX, radiusY, startAngle, endAngle);
    }
    else
    {
      SkMatrix rotated, rotatedInvert;
      rotated.setRotate(rotation, x, y);
      rotatedInvert.setRotate(-rotation, x, y);
      skPath->transform(rotatedInvert);
      ellipseHelper(skPath, x, y, radiusX, radiusY, startAngle, endAngle);
      skPath->transform(rotated);
    }
  }
}
