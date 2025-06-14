#pragma once

#include <napi.h>
#include <skia/include/core/SkData.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkBitmap.h>
#include "client/canvas/canvas.hpp"
#include "client/dom/dom_event_target.hpp"
#include "./image_source.hpp"
#include "../dom/event_target-inl.hpp"

namespace canvasbinding
{
  template <typename Context2dType, typename CanvasType>
  class CanvasWrap
  {
  public:
    CanvasWrap()
    {
    }
    CanvasWrap(std::shared_ptr<CanvasType> canvasImpl)
        : canvasImpl_(canvasImpl)
    {
    }

  public:
    shared_ptr<CanvasType> canvasImpl()
    {
      return canvasImpl_;
    }
    void setCanvasImpl(shared_ptr<CanvasType> canvasImpl)
    {
      canvasImpl_ = canvasImpl;
    }

  protected:
    Napi::Value GetContext(const Napi::CallbackInfo &info);

  private:
    std::shared_ptr<CanvasType> canvasImpl_;
  };

  class OffscreenCanvasRenderingContext2D;
  class OffscreenCanvas : public Napi::ObjectWrap<OffscreenCanvas>,
                          public CanvasWrap<OffscreenCanvasRenderingContext2D, canvas::OffscreenCanvas>,
                          public ImageSourceWrap<canvas::OffscreenCanvas>
  {
    friend class CanvasRenderingContext2D;
    friend class HTMLRenderingContext;

  public:
    static void Init(Napi::Env env, Napi::Object exports);
    OffscreenCanvas(const Napi::CallbackInfo &info);
    ~OffscreenCanvas();

  private:
    Napi::Value WidthGetter(const Napi::CallbackInfo &info);
    void WidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value HeightGetter(const Napi::CallbackInfo &info);
    void HeightSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value ConvertToBlob(const Napi::CallbackInfo &info);

  public:
    uint32_t width();
    uint32_t height();

  public:
    static thread_local Napi::FunctionReference *constructor;
  };

  /**
   * The `ReadOnlyScreenCanvas` class is introduced by JSAR, it represents the canvas corresponding to the device screen, and this canvas is
   * read-only.
   */
  class ReadOnlyScreenCanvas : public dombinding::EventTargetWrap<ReadOnlyScreenCanvas, dom::DOMEventTarget>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, uint32_t width, uint32_t height);
    ReadOnlyScreenCanvas(const Napi::CallbackInfo &info);
    ~ReadOnlyScreenCanvas();

  private:
    Napi::Value WidthGetter(const Napi::CallbackInfo &info);
    Napi::Value HeightGetter(const Napi::CallbackInfo &info);

  private:
    uint32_t width_;
    uint32_t height_;

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}

#include "./canvas-inl.hpp"
