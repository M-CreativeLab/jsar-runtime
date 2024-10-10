#include "path2d.hpp"

namespace canvasbinding
{
  thread_local Napi::FunctionReference *Path2D::constructor;
  void Path2D::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::HandleScope scope(env);
    auto properties = {
        InstanceMethod("addPath", &Path2D::AddPath),
        InstanceMethod("closePath", &Path2D::ClosePath),
        InstanceMethod("moveTo", &Path2D::MoveTo),
        InstanceMethod("lineTo", &Path2D::LineTo),
        InstanceMethod("bezierCurveTo", &Path2D::BezierCurveTo),
        InstanceMethod("quadraticCurveTo", &Path2D::QuadraticCurveTo),
        InstanceMethod("arc", &Path2D::Arc),
        InstanceMethod("arcTo", &Path2D::ArcTo),
        InstanceMethod("ellipse", &Path2D::Ellipse),
        InstanceMethod("rect", &Path2D::Rect),
        InstanceMethod("roundRect", &Path2D::RoundRect)};
    Napi::Function func = DefineClass(env, "Path2D", properties);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("Path2D", func);
    env.Global().Set("Path2D", func);
  }

  Path2D::Path2D(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Path2D>(info)
  {
    path2dImpl = std::make_shared<canvas::Path2D>();
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
