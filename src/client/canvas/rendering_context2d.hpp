#pragma once

#include <vector>
#include <memory>
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

#include "./rendering_context.hpp"
#include "./image_source.hpp"
#include "./image_data.hpp"

namespace canvas
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

  template <typename CanvasType>
  class CanvasRenderingContext2D : public RenderingContextBase<CanvasType>
  {
  public:
    using RenderingContextBase<CanvasType>::RenderingContextBase;
    CanvasRenderingContext2D(std::weak_ptr<CanvasType> canvasRef);

  public:
    bool setFillStyle(const std::string &style);
    bool setStrokeStyle(const std::string &style);
    std::string getFont();
    bool setFont(const std::string &fontStr);
    float getGlobalAlpha();
    bool setGlobalAlpha(float alpha);
    std::string getGlobalCompositeOperation();
    bool setGlobalCompositeOperation(const std::string &str);
    std::string getTextAlign();
    bool setTextAlign(const std::string &str);
    std::string getTextBaseline();
    bool setTextBaseline(const std::string &str);
    float getLineWidth();
    bool setLineWidth(float width);
    std::string getLineCap();
    bool setLineCap(const std::string &str);
    std::string getLineJoin();
    bool setLineJoin(const std::string &str);

  public:
    void fill();
    void fillRect(float x, float y, float width, float height);
    void fillText(const std::string &text, float x, float y);
    void stroke();
    void clearRect(float x, float y, float width, float height);
    void setLineDash(const std::vector<float> &segments);
    void beginPath();
    void closePath();
    void moveTo(float x, float y);
    void lineTo(float x, float y);
    void bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y);
    void quadraticCurveTo(float cpx, float cpy, float x, float y);
    void arc(float x, float y, float radius, float startAngle, float endAngle, bool ccw);
    void arcTo(float x1, float y1, float x2, float y2, float radius);
    TextMetrics measureText(const std::string &text);
    void transform(float a, float b, float c, float d, float e, float f);
    void setTransform(float a, float b, float c, float d, float e, float f);
    void scale(float sx, float sy);
    void rotate(float angle);
    void translate(float tx, float ty);
    void drawImage(std::shared_ptr<ImageSource> image, SkRect dstRect, SkRect srcRect);
    void drawImage(std::shared_ptr<ImageSource> image, float dx, float dy);
    void drawImage(std::shared_ptr<ImageSource> image, float dx, float dy, float dWidth, float dHeight);
    void drawImage(std::shared_ptr<ImageSource> image,
                   float sx, float sy, float sWidth, float sHeight,
                   float dx, float dy, float dWidth, float dHeight);
    std::shared_ptr<ImageData> createImageData(float width, float height, const std::string &colorSpace = "srgb");
    std::shared_ptr<ImageData> createImageData(std::shared_ptr<ImageData> otherImageData);
    std::shared_ptr<ImageData> getImageData(float x, float y, float width, float height);
    bool putImageData(std::shared_ptr<ImageSource> image, float dx, float dy);
    bool putImageData(std::shared_ptr<ImageSource> image,
                      float dx, float dy,
                      float dirtyX, float dirtyY, float dirtyWidth, float dirtyHeight);
    void save();
    void restore();

  private:
    SkPaint getFillPaint();
    SkPaint getStrokePaint();
    SkPaint *getShadowPaint(SkPaint &basePaint);
    void closeSkPath(std::shared_ptr<SkPath> path);
    bool ellipseToSkPath(std::shared_ptr<SkPath> path, float x, float y, float radiusX, float radiusY, float rotation,
                         float startAngle, float endAngle, bool ccw);

  private:
    SkCanvas *skCanvas;
    std::shared_ptr<SkPaint> skPaint;
    std::shared_ptr<SkFont> skFont;

  private:
    std::shared_ptr<SkPath> currentPath = nullptr;
    SkMatrix currentTransform = SkMatrix::I();

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
  };
}

#include "./rendering_context2d-inl.hpp"
