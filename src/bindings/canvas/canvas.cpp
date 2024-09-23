#include <skia/include/core/SkStream.h>
#include <skia/include/encode/SkEncoder.h>
#include <skia/include/encode/SkPngEncoder.h>

#include "canvas.hpp"
#include "rendering_context2d.hpp"
#include "html_rendering_context.hpp"
#include "debug.hpp"

namespace canvasbinding
{
  Napi::FunctionReference *OffscreenCanvas::constructor;

  void OffscreenCanvas::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "OffscreenCanvas",
                                     {InstanceMethod("getContext", &OffscreenCanvas::GetContext),
                                      InstanceAccessor("width", &OffscreenCanvas::WidthGetter, &OffscreenCanvas::WidthSetter),
                                      InstanceAccessor("height", &OffscreenCanvas::HeightGetter, &OffscreenCanvas::HeightSetter)});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    exports.Set("OffscreenCanvas", tpl);
  }

  OffscreenCanvas::OffscreenCanvas(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<OffscreenCanvas>(info),
        CanvasWrap<OffscreenCanvasRenderingContext2D, canvas::OffscreenCanvas>(),
        ImageSourceWrap<canvas::OffscreenCanvas>()
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2)
    {
      Napi::TypeError::New(env, "2 arguments expected").ThrowAsJavaScriptException();
      return;
    }

    auto width = info[0].ToNumber().Uint32Value();
    auto height = info[1].ToNumber().Uint32Value();
    dataImpl = make_shared<canvas::OffscreenCanvas>(width, height);
    setCanvasImpl(dataImpl);
  }

  OffscreenCanvas::~OffscreenCanvas()
  {
  }

  Napi::Value OffscreenCanvas::WidthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, dataImpl->width());
  }

  void OffscreenCanvas::WidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    dataImpl->setWidth(value.ToNumber().Uint32Value());
  }

  Napi::Value OffscreenCanvas::HeightGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, dataImpl->height());
  }

  void OffscreenCanvas::HeightSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    dataImpl->setHeight(value.ToNumber().Uint32Value());
  }

  uint32_t OffscreenCanvas::width()
  {
    return dataImpl->width();
  }

  uint32_t OffscreenCanvas::height()
  {
    return dataImpl->height();
  }
} // namespace webgl
