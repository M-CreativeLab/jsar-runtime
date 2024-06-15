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

  OffscreenCanvas::OffscreenCanvas(const Napi::CallbackInfo &info) : Napi::ObjectWrap<OffscreenCanvas>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2)
    {
      Napi::TypeError::New(env, "2 arguments expected").ThrowAsJavaScriptException();
      return;
    }

    width = info[0].ToNumber().Uint32Value();
    height = info[1].ToNumber().Uint32Value();
  }

  OffscreenCanvas::~OffscreenCanvas()
  {
    if (skBitmap != nullptr)
    {
      delete skBitmap;
      skBitmap = nullptr;
    }
  }

  Napi::Value OffscreenCanvas::WidthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, width);
  }

  void OffscreenCanvas::WidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (!currentContext2d.IsEmpty())
      DEBUG(LOG_TAG_CLIENT_CANVAS, "Trying to change width of canvas with active 2d context.");
    width = value.ToNumber().Uint32Value();
  }

  Napi::Value OffscreenCanvas::HeightGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(env, height);
  }

  void OffscreenCanvas::HeightSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (!currentContext2d.IsEmpty())
      DEBUG(LOG_TAG_CLIENT_CANVAS, "Trying to change height of canvas with active 2d context.");
    height = value.ToNumber().Uint32Value();
  }

  Napi::Value OffscreenCanvas::GetContext(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto type = info[0].ToString().Utf8Value();
    if (type == "2d")
    {
      if (!currentContext2d.IsEmpty())
        return currentContext2d.Value();

      resetSkSurface();
      auto context = CanvasRenderingContext2D::NewInstance(env, this);
      currentContext2d = Napi::Persistent(context);
      return context;
    }
    else if (type == "jsar:htmlrenderer")
    {
      if (!htmlRenderingContext.IsEmpty())
        return htmlRenderingContext.Value();

      resetSkSurface();
      auto context = HTMLRenderingContext::NewInstance(env, this);
      htmlRenderingContext = Napi::Persistent(context);
      return context;
    }
    else
    {
      // TODO: support other context types like webgl
      string msg = "Unsupported context type: (" + type + ")";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }
  }

  uint32_t OffscreenCanvas::getWidth()
  {
    return width;
  }

  uint32_t OffscreenCanvas::getHeight()
  {
    return height;
  }

  SkBitmap *OffscreenCanvas::getSkBitmap()
  {
    auto skCanvas = skSurface->getCanvas();
    if (skCanvas == nullptr)
      return nullptr;
    if (!skCanvas->readPixels(*skBitmap, 0, 0))
      return nullptr;
    return skBitmap;
  }

  sk_sp<SkSurface> &OffscreenCanvas::getSkSurface()
  {
    return skSurface;
  }

  void OffscreenCanvas::resetSkSurface()
  {
    auto imageInfo = SkImageInfo::MakeN32Premul(width, height);
    {
      // Reset SkSurface
      if (skSurface != nullptr)
        skSurface.reset();
      skSurface = SkSurfaces::Raster(imageInfo);
    }
    {
      // Reset SkBitmap
      if (skBitmap != nullptr)
        delete skBitmap;
      skBitmap = new SkBitmap();
      skBitmap->allocN32Pixels(width, height);
    }
  }

} // namespace webgl
