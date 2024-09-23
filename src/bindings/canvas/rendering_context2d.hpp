#pragma once

#include <memory>
#include <napi.h>

#include "client/per_process.hpp"
#include "client/canvas/canvas.hpp"
#include "client/canvas/rendering_context2d.hpp"
#include "./canvas.hpp"

namespace dombinding
{
  class HTMLCanvasElement;
}

namespace canvasbinding
{
  template <typename ObjectType, typename CanvasType>
  class CanvasRenderingContext2DBase : public Napi::ObjectWrap<ObjectType>
  {
  public:
    /**
     * Create a new instance of `CanvasRenderingContext2D` with the given context implementation instance.
     *
     * @param env The N-API environment.
     * @param contextImpl The context implementation instance.
     * @param canvas The canvas element.
     * @returns The new instance of `CanvasRenderingContext2D`.
     */
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<canvas::RenderingContextBase<CanvasType>> contextImpl, Napi::Value canvasValue);
    static std::vector<Napi::ClassPropertyDescriptor<ObjectType>> GetClassProperties();
    CanvasRenderingContext2DBase(const Napi::CallbackInfo &info);

  private:
    Napi::Value Fill(const Napi::CallbackInfo &info);
    Napi::Value FillRect(const Napi::CallbackInfo &info);
    Napi::Value FillText(const Napi::CallbackInfo &info);
    Napi::Value Stroke(const Napi::CallbackInfo &info);
    Napi::Value StrokeRect(const Napi::CallbackInfo &info);
    Napi::Value StrokeText(const Napi::CallbackInfo &info);
    Napi::Value ClearRect(const Napi::CallbackInfo &info);
    Napi::Value SetLineDash(const Napi::CallbackInfo &info);

  private: // Path methods
    Napi::Value BeginPath(const Napi::CallbackInfo &info);
    Napi::Value ClosePath(const Napi::CallbackInfo &info);
    Napi::Value MoveTo(const Napi::CallbackInfo &info);
    Napi::Value LineTo(const Napi::CallbackInfo &info);
    Napi::Value BezierCurveTo(const Napi::CallbackInfo &info);
    Napi::Value QuadraticCurveTo(const Napi::CallbackInfo &info);
    Napi::Value Arc(const Napi::CallbackInfo &info);
    Napi::Value ArcTo(const Napi::CallbackInfo &info);
    Napi::Value Ellipse(const Napi::CallbackInfo &info);
    Napi::Value Rect(const Napi::CallbackInfo &info);

  private: // Image methods
    Napi::Value DrawImage(const Napi::CallbackInfo &info);
    Napi::Value CreateImageData(const Napi::CallbackInfo &info);
    Napi::Value GetImageData(const Napi::CallbackInfo &info);
    Napi::Value PutImageData(const Napi::CallbackInfo &info);

  private: // Text methods
    Napi::Value MeasureText(const Napi::CallbackInfo &info);

  private: // Transform methods
    Napi::Value Transform(const Napi::CallbackInfo &info);
    Napi::Value SetTransform(const Napi::CallbackInfo &info);
    Napi::Value Rotate(const Napi::CallbackInfo &info);
    Napi::Value Scale(const Napi::CallbackInfo &info);
    Napi::Value Translate(const Napi::CallbackInfo &info);

  private: // State
    Napi::Value Save(const Napi::CallbackInfo &info);
    Napi::Value Restore(const Napi::CallbackInfo &info);

  private: // Getter & Setter
    Napi::Value CanvasGetter(const Napi::CallbackInfo &info);
    Napi::Value FillStyleGetter(const Napi::CallbackInfo &info);
    void FillStyleSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value StrokeStyleGetter(const Napi::CallbackInfo &info);
    void StrokeStyleSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value FontGetter(const Napi::CallbackInfo &info);
    void FontSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value GlobalAlphaGetter(const Napi::CallbackInfo &info);
    void GlobalAlphaSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value GlobalCompositeOperationGetter(const Napi::CallbackInfo &info);
    void GlobalCompositeOperationSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value TextAlignGetter(const Napi::CallbackInfo &info);
    void TextAlignSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value TextBaselineGetter(const Napi::CallbackInfo &info);
    void TextBaselineSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value LineWidthGetter(const Napi::CallbackInfo &info);
    void LineWidthSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value LineCapGetter(const Napi::CallbackInfo &info);
    void LineCapSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value LineJoinGetter(const Napi::CallbackInfo &info);
    void LineJoinSetter(const Napi::CallbackInfo &info, const Napi::Value &value);

  private:
    Napi::ObjectReference canvasRef;
    std::shared_ptr<canvas::CanvasRenderingContext2D<CanvasType>> contextImpl;
  };

  class CanvasRenderingContext2D : public CanvasRenderingContext2DBase<CanvasRenderingContext2D, canvas::Canvas>
  {
  public:
    using CanvasRenderingContext2DBase::CanvasRenderingContext2DBase;
    static void Init(Napi::Env env);
    /**
     * Create a new instance of `CanvasRenderingContext2D`.
     *
     * @param env The N-API environment.
     * @param contextImpl The context implementation instance.
     * @param canvasValue The canvas element to create the rendering context for.
     * @returns The new instance of `CanvasRenderingContext2D` or null if something went wrong.
     */
    static Napi::Object NewInstance(Napi::Env env,
                                    std::shared_ptr<canvas::RenderingContextBase<canvas::Canvas>> contextImpl,
                                    Napi::Value canvasValue);

  public:
    static Napi::FunctionReference *constructor;
  };

  class OffscreenCanvasRenderingContext2D : public CanvasRenderingContext2DBase<OffscreenCanvasRenderingContext2D, canvas::OffscreenCanvas>
  {
  public:
    using CanvasRenderingContext2DBase::CanvasRenderingContext2DBase;
    static std::vector<Napi::ClassPropertyDescriptor<OffscreenCanvasRenderingContext2D>> GetClassProperties();
    static void Init(Napi::Env env);

    /**
     * Create a new instance of `OffscreenCanvasRenderingContext2D`.
     *
     * @param env The N-API environment.
     * @param contextImpl The context implementation instance.
     * @param canvasValue The offscreen canvas element to create the rendering context for.
     */
    static Napi::Object NewInstance(Napi::Env env,
                                    std::shared_ptr<canvas::RenderingContextBase<canvas::OffscreenCanvas>> contextImpl,
                                    Napi::Value canvasValue);

  private:
    Napi::Value Commit(const Napi::CallbackInfo &info);

  public:
    static Napi::FunctionReference *constructor;
  };
}
