#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/canvas/canvas.hpp>
#include <client/canvas/rendering_context2d.hpp>

namespace script_bindings
{
  namespace canvas_bindings
  {
    using NativeCanvasRenderingContext2D = ::canvas::CanvasRenderingContext2D<::canvas::Canvas>;

    class CanvasRenderingContext2D;
    using CanvasRenderingContext2DBase = scripting_base::ObjectWrap<CanvasRenderingContext2D,
                                                                    NativeCanvasRenderingContext2D>;

    /**
     * CanvasRenderingContext2D wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps canvas::RenderingContext2D objects for use in V8 JavaScript execution contexts.
     * It provides the standard Canvas 2D API for drawing operations.
     */
    class CanvasRenderingContext2D : public CanvasRenderingContext2DBase
    {
      using CanvasRenderingContext2DBase::ObjectWrap;

    public:
      /**
       * The name of the CanvasRenderingContext2D class for V8.
       */
      static std::string Name()
      {
        return "CanvasRenderingContext2D";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<NativeCanvasRenderingContext2D> nativeRenderingContext);

    public:
      CanvasRenderingContext2D(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Drawing rectangles
      void FillRect(const v8::FunctionCallbackInfo<v8::Value> &info);
      void StrokeRect(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ClearRect(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Drawing text
      void FillText(const v8::FunctionCallbackInfo<v8::Value> &info);
      void StrokeText(const v8::FunctionCallbackInfo<v8::Value> &info);
      void MeasureText(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Line caps/joins
      void LineCap(const v8::FunctionCallbackInfo<v8::Value> &info);
      void LineJoin(const v8::FunctionCallbackInfo<v8::Value> &info);
      void LineWidth(const v8::FunctionCallbackInfo<v8::Value> &info);
      void MiterLimit(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Fill and stroke styles
      void FillStyleGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void FillStyleSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void StrokeStyleGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void StrokeStyleSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Shadows
      void ShadowBlurGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ShadowBlurSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void ShadowColorGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ShadowColorSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void ShadowOffsetXGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ShadowOffsetXSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void ShadowOffsetYGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void ShadowOffsetYSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Drawing paths
      void BeginPath(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ClosePath(const v8::FunctionCallbackInfo<v8::Value> &info);
      void MoveTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      void LineTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      void BezierCurveTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      void QuadraticCurveTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Arc(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ArcTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Ellipse(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Rect(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Drawing path methods
      void Fill(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Stroke(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Clip(const v8::FunctionCallbackInfo<v8::Value> &info);
      void IsPointInPath(const v8::FunctionCallbackInfo<v8::Value> &info);
      void IsPointInStroke(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Transformations
      void Scale(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Rotate(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Translate(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Transform(const v8::FunctionCallbackInfo<v8::Value> &info);
      void SetTransform(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ResetTransform(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Drawing images
      void DrawImage(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Pixel manipulation
      void CreateImageData(const v8::FunctionCallbackInfo<v8::Value> &info);
      void GetImageData(const v8::FunctionCallbackInfo<v8::Value> &info);
      void PutImageData(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Canvas state
      void Save(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Restore(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Canvas element property
      void CanvasGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

      void LineWidthGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void LineWidthSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void LineCapGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void LineCapSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void GlobalAlphaGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void GlobalAlphaSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void GlobalCompositeOperationGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void GlobalCompositeOperationSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
    };
  }
}
