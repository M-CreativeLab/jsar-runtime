#pragma once

#include <napi.h>
#include <skia/include/core/SkPath.h>

namespace bindings
{
  namespace canvas
  {
    class Path2D : public Napi::ObjectWrap<Path2D>
    {
    public:
      static void Init(Napi::Env env, Napi::Object exports);
      Path2D(const Napi::CallbackInfo &info);
      ~Path2D();

    private:
      Napi::Value AddPath(const Napi::CallbackInfo &info);
      Napi::Value ClosePath(const Napi::CallbackInfo &info);
      Napi::Value MoveTo(const Napi::CallbackInfo &info);
      Napi::Value LineTo(const Napi::CallbackInfo &info);
      Napi::Value BezierCurveTo(const Napi::CallbackInfo &info);
      Napi::Value QuadraticCurveTo(const Napi::CallbackInfo &info);
      Napi::Value Arc(const Napi::CallbackInfo &info);
      Napi::Value ArcTo(const Napi::CallbackInfo &info);
      Napi::Value Ellipse(const Napi::CallbackInfo &info);
      Napi::Value Rect(const Napi::CallbackInfo &info);
      Napi::Value RoundRect(const Napi::CallbackInfo &info);

    private:
      SkPath skPath;

    public:
      static Napi::FunctionReference *constructor;
    };
  }
}
