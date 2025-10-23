#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/canvas/canvas.hpp>
#include <client/canvas/rendering_context2d.hpp>

namespace script_bindings
{
  namespace canvas_bindings
  {
    template <typename T, typename CanvasType>
    class CanvasRenderingContext2DBase
        : public scripting_base::ObjectWrap<T, canvas::CanvasRenderingContext2D<CanvasType>>
    {
      using scripting_base::ObjectWrap<T, canvas::CanvasRenderingContext2D<CanvasType>>::ObjectWrap;

    public:
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
      {
        v8::HandleScope scope(isolate);
        v8::Local<v8::ObjectTemplate> prototype = tpl->PrototypeTemplate();
        v8::Local<v8::ObjectTemplate> instance = tpl->InstanceTemplate();

        // Drawing rectangles
        T::InstanceMethod(isolate, prototype, "fillRect", &T::FillRect);
        T::InstanceMethod(isolate, prototype, "strokeRect", &T::StrokeRect);
        T::InstanceMethod(isolate, prototype, "clearRect", &T::ClearRect);

        // Drawing text
        T::InstanceMethod(isolate, prototype, "fillText", &T::FillText);
        T::InstanceMethod(isolate, prototype, "strokeText", &T::StrokeText);
        T::InstanceMethod(isolate, prototype, "measureText", &T::MeasureText);
        T::InstanceAccessor(isolate, instance, "font", &T::FontGetter, &T::FontSetter);
        T::InstanceAccessor(isolate, instance, "textAlign", &T::TextAlignGetter, &T::TextAlignSetter);
        T::InstanceAccessor(isolate, instance, "textBaseline", &T::TextBaselineGetter, &T::TextBaselineSetter);

        // Paths
        T::InstanceMethod(isolate, prototype, "beginPath", &T::BeginPath);
        T::InstanceMethod(isolate, prototype, "closePath", &T::ClosePath);
        T::InstanceMethod(isolate, prototype, "moveTo", &T::MoveTo);
        T::InstanceMethod(isolate, prototype, "lineTo", &T::LineTo);
        T::InstanceMethod(isolate, prototype, "bezierCurveTo", &T::BezierCurveTo);
        T::InstanceMethod(isolate, prototype, "quadraticCurveTo", &T::QuadraticCurveTo);
        T::InstanceMethod(isolate, prototype, "arc", &T::Arc);
        T::InstanceMethod(isolate, prototype, "rect", &T::Rect);
        T::InstanceMethod(isolate, prototype, "ellipse", &T::Ellipse);

        // Drawing paths
        T::InstanceMethod(isolate, prototype, "fill", &T::Fill);
        T::InstanceMethod(isolate, prototype, "stroke", &T::Stroke);

        // Transformations
        T::InstanceMethod(isolate, prototype, "scale", &T::Scale);
        T::InstanceMethod(isolate, prototype, "rotate", &T::Rotate);
        T::InstanceMethod(isolate, prototype, "translate", &T::Translate);
        T::InstanceMethod(isolate, prototype, "transform", &T::Transform);
        T::InstanceMethod(isolate, prototype, "setTransform", &T::SetTransform);
        T::InstanceMethod(isolate, prototype, "resetTransform", &T::ResetTransform);

        // Image operations
        T::InstanceMethod(isolate, prototype, "drawImage", &T::DrawImage);
        T::InstanceMethod(isolate, prototype, "createImageData", &T::CreateImageData);
        T::InstanceMethod(isolate, prototype, "getImageData", &T::GetImageData);
        T::InstanceMethod(isolate, prototype, "putImageData", &T::PutImageData);

        // State management
        T::InstanceMethod(isolate, prototype, "save", &T::Save);
        T::InstanceMethod(isolate, prototype, "restore", &T::Restore);

        // Line dash
        T::InstanceAccessor(isolate, prototype, "lineDashOffset", &T::LineDashOffsetGetter, &T::LineDashOffsetSetter);
        T::InstanceMethod(isolate, prototype, "getLineDash", &T::GetLineDash);
        T::InstanceMethod(isolate, prototype, "setLineDash", &T::SetLineDash);

        // Properties
        T::InstanceReadonlyAccessor(isolate, instance, "canvas", &T::CanvasGetter);
        T::InstanceAccessor(isolate, instance, "fillStyle", &T::FillStyleGetter, &T::FillStyleSetter);
        T::InstanceAccessor(isolate, instance, "strokeStyle", &T::StrokeStyleGetter, &T::StrokeStyleSetter);
        T::InstanceAccessor(isolate, instance, "lineWidth", &T::LineWidthGetter, &T::LineWidthSetter);
        T::InstanceAccessor(isolate, instance, "globalAlpha", &T::GlobalAlphaGetter, &T::GlobalAlphaSetter);
        T::InstanceAccessor(isolate,
                            instance,
                            "globalCompositeOperation",
                            &T::GlobalCompositeOperationGetter,
                            &T::GlobalCompositeOperationSetter);
      }

    protected:
      // Drawing rectangles
      void FillRect(const v8::FunctionCallbackInfo<v8::Value> &info);
      void StrokeRect(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ClearRect(const v8::FunctionCallbackInfo<v8::Value> &info);

      // Drawing text
      void FillText(const v8::FunctionCallbackInfo<v8::Value> &info);
      void StrokeText(const v8::FunctionCallbackInfo<v8::Value> &info);
      void MeasureText(const v8::FunctionCallbackInfo<v8::Value> &info);
      void FontGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void FontSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void TextAlignGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void TextAlignSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void TextBaselineGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void TextBaselineSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

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
      void Rect(const v8::FunctionCallbackInfo<v8::Value> &info);
      void Ellipse(const v8::FunctionCallbackInfo<v8::Value> &info);

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

      // Line Dash
      void LineDashOffsetGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void LineDashOffsetSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void GetLineDash(const v8::FunctionCallbackInfo<v8::Value> &info);
      void SetLineDash(const v8::FunctionCallbackInfo<v8::Value> &info);

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

    class CanvasRenderingContext2D : public CanvasRenderingContext2DBase<CanvasRenderingContext2D, canvas::Canvas>
    {
      using CanvasRenderingContext2DBase::CanvasRenderingContext2DBase;

    public:
      static std::string Name()
      {
        return "CanvasRenderingContext2D";
      }

    public:
      CanvasRenderingContext2D(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);
    };
  }
}
