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

      /**
       * Configure the V8 function template with CanvasRenderingContext2D properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 CanvasRenderingContext2D instance from a native canvas::RenderingContext2D.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<NativeCanvasRenderingContext2D> nativeContext);

      /**
       * Initialize the CanvasRenderingContext2D class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      CanvasRenderingContext2D(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Drawing rectangles
      static void FillRect(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void StrokeRect(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ClearRect(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Drawing text
      static void FillText(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void StrokeText(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void MeasureText(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Line caps/joins
      static void LineCap(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void LineJoin(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void LineWidth(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void MiterLimit(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Fill and stroke styles
      static void FillStyleGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void FillStyleSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void StrokeStyleGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void StrokeStyleSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Shadows
      static void ShadowBlurGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void ShadowBlurSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void ShadowColorGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void ShadowColorSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void ShadowOffsetXGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void ShadowOffsetXSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      static void ShadowOffsetYGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void ShadowOffsetYSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Drawing paths
      static void BeginPath(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ClosePath(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void MoveTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void LineTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void BezierCurveTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void QuadraticCurveTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Arc(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ArcTo(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Ellipse(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Rect(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Drawing path methods
      static void Fill(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Stroke(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Clip(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void IsPointInPath(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void IsPointInStroke(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Transformations
      static void Scale(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Rotate(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Translate(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Transform(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void SetTransform(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void ResetTransform(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Drawing images
      static void DrawImage(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Pixel manipulation
      static void CreateImageData(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void GetImageData(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void PutImageData(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Canvas state
      static void Save(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void Restore(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Canvas element property
      static void CanvasGetter(
        v8::Local<v8::String> property,
        const v8::PropertyCallbackInfo<v8::Value> &info);
      
      static void LineWidthGetter(
        v8::Local<v8::String> property,
        const v8::PropertyCallbackInfo<v8::Value> &info);
      static void LineWidthSetter(
        v8::Local<v8::String> property,
        v8::Local<v8::Value> value,
        const v8::PropertyCallbackInfo<void> &info);
      static void LineCapGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void LineCapSetter(
        v8::Local<v8::String> property,
        v8::Local<v8::Value> value,
        const v8::PropertyCallbackInfo<void> &info);
      static void GlobalAlphaGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void GlobalAlphaSetter(
        v8::Local<v8::String> property,
        v8::Local<v8::Value> value,
        const v8::PropertyCallbackInfo<void> &info);
      static void GlobalCompositeOperationGetter(
        v8::Local<v8::String> property,
        const v8::PropertyCallbackInfo<v8::Value> &info);
      static void GlobalCompositeOperationSetter(
        v8::Local<v8::String> property,
        v8::Local<v8::Value> value,
        const v8::PropertyCallbackInfo<void> &info);
    };
  }
}
