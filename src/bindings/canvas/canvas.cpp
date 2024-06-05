#include "canvas.hpp"
#include "rendering_context2d.hpp"

namespace canvasbinding
{
  Napi::FunctionReference *OffscreenCanvas::constructor;

  void OffscreenCanvas::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "OffscreenCanvas",
                                     {InstanceMethod("getContext", &OffscreenCanvas::GetContext)});
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

    auto imageInfo = SkImageInfo::MakeN32Premul(width, height);
    skSurface = SkSurfaces::Raster(imageInfo);
    skBitmap = new SkBitmap();
    skBitmap->allocN32Pixels(width, height);
  }

  OffscreenCanvas::~OffscreenCanvas()
  {
    if (skBitmap != nullptr)
    {
      delete skBitmap;
      skBitmap = nullptr;
    }
  }

  Napi::Value OffscreenCanvas::GetContext(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto type = info[0].ToString().Utf8Value();
    if (type == "2d")
    {
      auto context = CanvasRenderingContext2D::NewInstance(env, this);
      currentContext = Napi::Persistent(context);
    }
    else
    {
      // TODO: support other context types like webgl
      Napi::TypeError::New(env, "Only 2d context is supported").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    return currentContext.Value();
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
    if (skSurface->getCanvas()->readPixels(*skBitmap, 0, 0))
      return skBitmap;
    else
      return nullptr;
  }

  sk_sp<SkSurface> &OffscreenCanvas::getSkSurface()
  {
    return skSurface;
  }

} // namespace webgl
