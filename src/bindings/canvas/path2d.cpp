#include "path2d.hpp"

namespace bindings
{
  namespace canvas
  {
    Napi::FunctionReference *Path2D::constructor;

    void Path2D::Init(Napi::Env env, Napi::Object exports)
    {
      Napi::Function func = DefineClass(env, "Path2D",
                                        {InstanceMethod("addPath", &Path2D::AddPath),
                                         InstanceMethod("closePath", &Path2D::ClosePath),
                                         InstanceMethod("moveTo", &Path2D::MoveTo),
                                         InstanceMethod("lineTo", &Path2D::LineTo),
                                         InstanceMethod("bezierCurveTo", &Path2D::BezierCurveTo),
                                         InstanceMethod("quadraticCurveTo", &Path2D::QuadraticCurveTo),
                                         InstanceMethod("arc", &Path2D::Arc),
                                         InstanceMethod("arcTo", &Path2D::ArcTo),
                                         InstanceMethod("ellipse", &Path2D::Ellipse),
                                         InstanceMethod("rect", &Path2D::Rect),
                                         InstanceMethod("roundRect", &Path2D::RoundRect)});
      constructor = new Napi::FunctionReference();
      *constructor = Napi::Persistent(func);
      exports.Set("Path2D", func);
    }

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

    void ellipseHelper(SkPath *skPath,
                       float x, float y, float radiusX, float radiusY,
                       float startAngle, float endAngle)
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

    void Path2D::ApplyArcToTangent(SkPath *skPath, SkScalar x1, SkScalar y1, SkScalar x2, SkScalar y2, SkScalar radius)
    {
      skPath->arcTo(x1, y1, x2, y2, radius);
    }

    void Path2D::Ellipse(SkPath *skPath,
                         float x, float y, float radiusX, float radiusY, float rotation,
                         float startAngle, float endAngle, bool ccw)
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

    Path2D::Path2D(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Path2D>(info)
    {
      // TODO
    }

    Path2D::~Path2D()
    {
      // TODO
    }

    Napi::Value Path2D::AddPath(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }

    Napi::Value Path2D::ClosePath(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }

    Napi::Value Path2D::MoveTo(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }

    Napi::Value Path2D::LineTo(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }

    Napi::Value Path2D::BezierCurveTo(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }

    Napi::Value Path2D::QuadraticCurveTo(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }

    Napi::Value Path2D::Arc(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }

    Napi::Value Path2D::ArcTo(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }

    Napi::Value Path2D::Ellipse(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }

    Napi::Value Path2D::Rect(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }

    Napi::Value Path2D::RoundRect(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      return env.Undefined();
    }
  }
}