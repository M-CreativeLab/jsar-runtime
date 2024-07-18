#pragma once

#include <napi.h>
#include <skia/include/core/SkData.h>
#include <skia/include/core/SkMatrix.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkFont.h>
#include <skia/include/core/SkFontMgr.h>
#include <skia/include/core/SkPath.h>
#include <skia/include/core/SkPathEffect.h>
#include <skia/include/core/SkTextBlob.h>
#include <skia/include/core/SkTypeface.h>
#include <glm/glm.hpp>

#include "canvas.hpp"
#include "client/per_process.hpp"

namespace canvasbinding
{
  enum class TextAlign
  {
    Start,
    End,
    Left,
    Right,
    Center
  };

  enum class TextBaseline
  {
    Top,
    Hanging,
    Middle,
    Alphabetic,
    Ideographic,
    Bottom
  };

  class TextMetrics
  {
  public:
    TextMetrics()
    {
    }

  public:
    float width;
    float actualBoundingBoxLeft;
    float actualBoundingBoxRight;
    float fontBoundingBoxAscent;
    float fontBoundingBoxDescent;
    float actualBoundingBoxAscent;
    float actualBoundingBoxDescent;
    float emHeightAscent;
    float emHeightDescent;
    float hangingBaseline;
    float alphabeticBaseline;
    float ideographicBaseline;
  };

  class CanvasRenderingContext2D : public Napi::ObjectWrap<CanvasRenderingContext2D>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    static Napi::Object NewInstance(Napi::Env env, OffscreenCanvas *canvas);
    CanvasRenderingContext2D(const Napi::CallbackInfo &info);
    ~CanvasRenderingContext2D();

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

  private:
    Napi::Value CanvasGetter(const Napi::CallbackInfo &info);
    Napi::Value CurrentTransformGetter(const Napi::CallbackInfo &info);
    void CurrentTransformSetter(const Napi::CallbackInfo &info, const Napi::Value &value);
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

  private: // State
    Napi::Value Save(const Napi::CallbackInfo &info);
    Napi::Value Restore(const Napi::CallbackInfo &info);

  private:
    SkPaint getFillPaint();
    SkPaint getStrokePaint();
    SkPaint *getShadowPaint(SkPaint &basePaint);
    TextMetrics measureText(const std::string &text);
    void closeSkPath(SkPath *path);
    bool ellipseToSkPath(SkPath *path, float x, float y, float radiusX, float radiusY, float rotation,
                         float startAngle, float endAngle, bool anticlockwise);

  private:
    Napi::ObjectReference *jsCanvas = nullptr;
    TrClientContextPerProcess *clientContext = nullptr;
    SkCanvas *skCanvas;
    SkPaint *skPaint;
    SkFont *skFont;

  private: // text & font
    std::string fontStr = "14px monospace";
    TextAlign textAlign = TextAlign::Start;
    TextBaseline textBaseline = TextBaseline::Alphabetic;

  private: // style
    SkColor fillStyle = SK_ColorBLACK;
    SkColor strokeStyle = SK_ColorBLACK;

  private:
    float shadowBlur = 0.0f;
    SkColor shadowColor = SK_ColorTRANSPARENT;
    glm::vec2 shadowOffset = glm::vec2(0.0f);
    float globalAlpha = 1.0f;
    float strokeWidth = 1.0f;
    SkBlendMode globalCompositeOperation = SkBlendMode::kSrcOver;
    std::vector<float> lineDash;

  private:
    SkPath *currentPath = nullptr;
    SkMatrix currentTransform = SkMatrix::I();

  public:
    static Napi::FunctionReference *constructor;
  };
}
