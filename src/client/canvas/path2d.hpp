#pragma once

#include <memory>
#include <skia/include/core/SkPath.h>

namespace canvas
{
  class Path2D
  {
  public:
    Path2D() {}

  public:
    static void ApplyArcToTangent(std::shared_ptr<SkPath> skPath, SkScalar x1, SkScalar y1, SkScalar x2, SkScalar y2, SkScalar radius);
    static void Ellipse(std::shared_ptr<SkPath> skPath,
                        float x, float y, float radiusX, float radiusY, float rotation,
                        float startAngle, float endAngle, bool ccw);

  public:
    void addPath();
    void closePath();
    void moveTo();
    void lineTo();
    void bezierCurveTo();
    void quadraticCurveTo();
    void arc();
    void arcTo();
    void ellipse();
    void rect();
    void roundRect();

  private:
    SkPath skPath;
  };
}
