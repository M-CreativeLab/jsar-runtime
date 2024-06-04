#pragma once

#include <napi.h>
#include <skia/include/core/SkData.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>

namespace canvasbinding
{
  class OffscreenCanvas : public Napi::ObjectWrap<OffscreenCanvas>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    OffscreenCanvas(const Napi::CallbackInfo &info);

  private:
    Napi::Value WidthGetter(const Napi::CallbackInfo &info);
    Napi::Value HeightGetter(const Napi::CallbackInfo &info);
    Napi::Value GetContext(const Napi::CallbackInfo &info);
    Napi::Value ConvertToBlob(const Napi::CallbackInfo &info);

  private:
    Napi::ObjectReference currentContext;
    uint32_t width;
    uint32_t height;
    sk_sp<SkSurface> skSurface;

  public:
    static Napi::FunctionReference *constructor;

    friend class CanvasRenderingContext2D;
  };
}
