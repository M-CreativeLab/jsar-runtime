#pragma once

#include <napi.h>
#include <skia/include/core/SkData.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkFont.h>
#include <glm/glm.hpp>
#include "canvas.hpp"

namespace canvasbinding
{
  class CanvasRenderingContext2D : public Napi::ObjectWrap<CanvasRenderingContext2D>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, OffscreenCanvas *canvas);
    CanvasRenderingContext2D(const Napi::CallbackInfo &info);

  private:
    Napi::Value Fill(const Napi::CallbackInfo &info);
    Napi::Value FillRect(const Napi::CallbackInfo &info);
    Napi::Value FillText(const Napi::CallbackInfo &info);
    Napi::Value Stroke(const Napi::CallbackInfo &info);
    Napi::Value StrokeRect(const Napi::CallbackInfo &info);
    Napi::Value StrokeText(const Napi::CallbackInfo &info);

  private:
    Napi::ObjectReference *jsCanvas;
    SkCanvas *skCanvas;
    SkPaint *skPaint;
    SkFont *skFont;
    glm::mat2x3 currentTransform = glm::mat2x3(1.0f);

  public:
    static Napi::FunctionReference *constructor;
  };
}
