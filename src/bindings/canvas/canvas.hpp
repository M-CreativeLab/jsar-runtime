#pragma once

#include <napi.h>
#include <skia/include/core/SkData.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkBitmap.h>

namespace canvasbinding
{
  class OffscreenCanvas : public Napi::ObjectWrap<OffscreenCanvas>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    OffscreenCanvas(const Napi::CallbackInfo &info);
    ~OffscreenCanvas();

  private:
    Napi::Value WidthGetter(const Napi::CallbackInfo &info);
    void WidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value HeightGetter(const Napi::CallbackInfo &info);
    void HeightSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value GetContext(const Napi::CallbackInfo &info);
    Napi::Value ConvertToBlob(const Napi::CallbackInfo &info);

  public:
    uint32_t getWidth();
    uint32_t getHeight();
    SkBitmap* getSkBitmap();
    sk_sp<SkSurface> &getSkSurface();

  private:
    Napi::ObjectReference currentContext2d;
    uint32_t width;
    uint32_t height;
    sk_sp<SkSurface> skSurface;
    SkBitmap* skBitmap;

  public:
    static Napi::FunctionReference *constructor;

    friend class CanvasRenderingContext2D;
  };
}
