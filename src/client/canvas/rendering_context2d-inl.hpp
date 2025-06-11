#include <skia/include/core/SkFontMetrics.h>
#include <skia/include/core/SkColorSpace.h>
#include <skia/include/effects/SkDashPathEffect.h>

#include <crates/bindings.hpp>
#include <common/font/parser.hpp>

#include "./rendering_context2d.hpp"
#include "./path2d.hpp"
#include "../per_process.hpp"

namespace canvas
{
  template <typename CanvasType>
  CanvasRenderingContext2D<CanvasType>::CanvasRenderingContext2D(std::shared_ptr<CanvasType> canvasRef)
      : RenderingContextBase<CanvasType>(RenderingContextType::RenderingContext2D, canvasRef)
  {
    reset(canvasRef->skSurface);

    skPaint = std::make_shared<SkPaint>();
    skPaint->setAntiAlias(true);
    skPaint->setStrokeMiter(10);
    skPaint->setBlendMode(globalCompositeOperation);
    skPaint->setColor(fillStyle);
    skPaint->setStrokeWidth(strokeWidth);

    auto fontMgr = this->clientContext->getFontCacheManager();
    skFont = std::make_shared<SkFont>(fontMgr.getTypeface(), 50);
    skFont->setSubpixel(true);
  }

  template <typename CanvasType>
  bool CanvasRenderingContext2D<CanvasType>::setFillStyle(const std::string &style)
  {
    auto color = crates::css2::parsing::parseColor(style);
    fillStyle = SkColorSetARGB(color.a(), color.r(), color.g(), color.b());
    return true;
  }

  template <typename CanvasType>
  bool CanvasRenderingContext2D<CanvasType>::setStrokeStyle(const std::string &style)
  {
    auto color = crates::css2::parsing::parseColor(style);
    strokeStyle = SkColorSetARGB(color.a(), color.r(), color.g(), color.b());
    return true;
  }

  template <typename CanvasType>
  std::string CanvasRenderingContext2D<CanvasType>::getFont()
  {
    return fontStr;
  }

  template <typename CanvasType>
  bool CanvasRenderingContext2D<CanvasType>::setFont(const std::string &str)
  {
    font::FontShorthandParser descriptor(str);
    if (descriptor.success)
    {
      auto fontMgr = this->clientContext->getFontCacheManager();
      auto typeface = fontMgr.getTypeface(descriptor);
      if (typeface)
      {
        skFont->setSize(descriptor.sizeInPx);
        skFont->setTypeface(typeface);
        fontStr = str;
        return true;
      }
      else
      {
        std::cerr << "Failed to find typeface for font: " << str << std::endl;
        return false;
      }
    }
    else
    {
      std::cerr << "Failed to parse font descriptor: " << str << std::endl;
      return false;
    }
  }

  template <typename CanvasType>
  float CanvasRenderingContext2D<CanvasType>::getGlobalAlpha()
  {
    return globalAlpha;
  }

  template <typename CanvasType>
  bool CanvasRenderingContext2D<CanvasType>::setGlobalAlpha(float alpha)
  {
    globalAlpha = alpha;
    return true;
  }

#define GCO_MAP(XX)                             \
  XX(SkBlendMode::kSrcOver, "source-over")      \
  XX(SkBlendMode::kDstOver, "destination-over") \
  XX(SkBlendMode::kSrc, "copy")                 \
  XX(SkBlendMode::kDst, "destination")          \
  XX(SkBlendMode::kClear, "clear")              \
  XX(SkBlendMode::kSrcIn, "source-in")          \
  XX(SkBlendMode::kDstIn, "destination-in")     \
  XX(SkBlendMode::kSrcOut, "source-out")        \
  XX(SkBlendMode::kDstOut, "destination-out")   \
  XX(SkBlendMode::kSrcATop, "source-atop")      \
  XX(SkBlendMode::kDstATop, "destination-atop") \
  XX(SkBlendMode::kXor, "xor")                  \
  XX(SkBlendMode::kPlus, "lighter")             \
  XX(SkBlendMode::kMultiply, "multiply")        \
  XX(SkBlendMode::kScreen, "screen")            \
  XX(SkBlendMode::kOverlay, "overlay")          \
  XX(SkBlendMode::kDarken, "darken")            \
  XX(SkBlendMode::kLighten, "lighten")          \
  XX(SkBlendMode::kColorDodge, "color-dodge")   \
  XX(SkBlendMode::kColorBurn, "color-burn")     \
  XX(SkBlendMode::kHardLight, "hard-light")     \
  XX(SkBlendMode::kSoftLight, "soft-light")     \
  XX(SkBlendMode::kDifference, "difference")    \
  XX(SkBlendMode::kExclusion, "exclusion")      \
  XX(SkBlendMode::kHue, "hue")                  \
  XX(SkBlendMode::kSaturation, "saturation")    \
  XX(SkBlendMode::kColor, "color")              \
  XX(SkBlendMode::kLuminosity, "luminosity")

  template <typename CanvasType>
  std::string CanvasRenderingContext2D<CanvasType>::getGlobalCompositeOperation()
  {
    switch (globalCompositeOperation)
    {
#define XX(mode, name) \
  case mode:           \
    return name;
      GCO_MAP(XX)
#undef XX
    default:
      return "";
    }
  }

  template <typename CanvasType>
  bool CanvasRenderingContext2D<CanvasType>::setGlobalCompositeOperation(const std::string &str)
  {
#define XX(mode, name)               \
  if (str == name)                   \
  {                                  \
    globalCompositeOperation = mode; \
    return true;                     \
  }
    GCO_MAP(XX)
#undef XX
    return false;
  }

#define TEXT_ALIGN_MAP(XX)      \
  XX(TextAlign::Start, "start") \
  XX(TextAlign::End, "end")     \
  XX(TextAlign::Left, "left")   \
  XX(TextAlign::Right, "right") \
  XX(TextAlign::Center, "center")

  template <typename CanvasType>
  std::string CanvasRenderingContext2D<CanvasType>::getTextAlign()
  {
    switch (textAlign)
    {
#define XX(id, name) \
  case id:           \
    return name;
      TEXT_ALIGN_MAP(XX)
#undef XX
    default:
      return "";
    }
  }

  template <typename CanvasType>
  bool CanvasRenderingContext2D<CanvasType>::setTextAlign(const std::string &str)
  {
#define XX(id, name) \
  if (str == name)   \
  {                  \
    textAlign = id;  \
    return true;     \
  }
    TEXT_ALIGN_MAP(XX)
#undef XX
    return false;
  }

#define TEXT_BASELINE_MAP(XX)                  \
  XX(TextBaseline::Top, "top")                 \
  XX(TextBaseline::Hanging, "hanging")         \
  XX(TextBaseline::Middle, "middle")           \
  XX(TextBaseline::Alphabetic, "alphabetic")   \
  XX(TextBaseline::Ideographic, "ideographic") \
  XX(TextBaseline::Bottom, "bottom")

  template <typename CanvasType>
  std::string CanvasRenderingContext2D<CanvasType>::getTextBaseline()
  {
    switch (textBaseline)
    {
#define XX(id, name) \
  case id:           \
    return name;
      TEXT_BASELINE_MAP(XX)
#undef XX
    default:
      return "";
    }
  }

  template <typename CanvasType>
  bool CanvasRenderingContext2D<CanvasType>::setTextBaseline(const std::string &str)
  {
#define XX(id, name)   \
  if (str == name)     \
  {                    \
    textBaseline = id; \
    return true;       \
  }
    TEXT_BASELINE_MAP(XX)
#undef XX
    return false;
  }

  template <typename CanvasType>
  float CanvasRenderingContext2D<CanvasType>::getLineWidth()
  {
    return skPaint->getStrokeWidth();
  }

  template <typename CanvasType>
  bool CanvasRenderingContext2D<CanvasType>::setLineWidth(float width)
  {
    strokeWidth = width;
    skPaint->setStrokeWidth(strokeWidth);
    return true;
  }

#define LINE_CAP_MAP(XX)           \
  XX(SkPaint::kButt_Cap, "butt")   \
  XX(SkPaint::kRound_Cap, "round") \
  XX(SkPaint::kSquare_Cap, "square")

  template <typename CanvasType>
  std::string CanvasRenderingContext2D<CanvasType>::getLineCap()
  {
    switch (skPaint->getStrokeCap())
    {
#define XX(id, name) \
  case id:           \
    return name;
      LINE_CAP_MAP(XX)
#undef XX
    default:
      return "";
    }
  }

  template <typename CanvasType>
  bool CanvasRenderingContext2D<CanvasType>::setLineCap(const std::string &str)
  {
#define XX(id, name)           \
  if (str == name)             \
  {                            \
    skPaint->setStrokeCap(id); \
    return true;               \
  }
    LINE_CAP_MAP(XX)
#undef XX
    return false;
  }

#define LINE_JOIN_MAP(XX)           \
  XX(SkPaint::kMiter_Join, "miter") \
  XX(SkPaint::kRound_Join, "round") \
  XX(SkPaint::kBevel_Join, "bevel")

  template <typename CanvasType>
  std::string CanvasRenderingContext2D<CanvasType>::getLineJoin()
  {
    switch (skPaint->getStrokeJoin())
    {
#define XX(id, name) \
  case id:           \
    return name;
      LINE_JOIN_MAP(XX)
#undef XX
    default:
      return "";
    }
  }

  template <typename CanvasType>
  bool CanvasRenderingContext2D<CanvasType>::setLineJoin(const std::string &str)
  {
#define XX(id, name)            \
  if (str == name)              \
  {                             \
    skPaint->setStrokeJoin(id); \
    return true;                \
  }
    LINE_JOIN_MAP(XX)
#undef XX
    return false;
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::fill()
  {
    if (TR_UNLIKELY(skCanvas == nullptr))
      return;

    auto fillPaint = getFillPaint();
    auto shadowPaint = getShadowPaint(fillPaint);
    if (shadowPaint != nullptr)
    {
      skCanvas->save();
      // TODO
      skCanvas->drawPath(*currentPath, *shadowPaint);
      skCanvas->restore();
      delete shadowPaint;
    }
    skCanvas->drawPath(*currentPath, fillPaint);
    this->notifyCanvasUpdated();
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::fillRect(float x, float y, float width, float height)
  {
    if (TR_UNLIKELY(skCanvas == nullptr))
      return;

    auto fillPaint = getFillPaint();
    // TODO: shadow painting
    skCanvas->drawRect(SkRect::MakeXYWH(x, y, width, height), fillPaint);
    this->notifyCanvasUpdated();
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::fillText(const std::string &text, float x, float y)
  {
    if (TR_UNLIKELY(skCanvas == nullptr))
      return;

    auto fillPaint = getFillPaint();
    auto textBlob = SkTextBlob::MakeFromString(text.c_str(), *skFont);

    /**
     * Adjust text's position based on `textAlign` and `textBaseline`.
     */
    auto textMetrics = measureText(text);
    switch (textAlign)
    {
    case TextAlign::Center:
      x -= textMetrics.width / 2;
      break;
    case TextAlign::Right:
      x -= textMetrics.width;
      break;
    case TextAlign::Start:
    case TextAlign::End:
    case TextAlign::Left:
    default:
      break;
    }

    switch (textBaseline)
    {
    case TextBaseline::Top:
      y -= textMetrics.actualBoundingBoxAscent;
      break;
    case TextBaseline::Middle:
      y -= (textMetrics.actualBoundingBoxAscent - textMetrics.actualBoundingBoxDescent) / 2;
      break;
    case TextBaseline::Bottom:
      y -= textMetrics.actualBoundingBoxDescent;
      break;
    case TextBaseline::Hanging:
    case TextBaseline::Alphabetic:
    case TextBaseline::Ideographic:
    default:
      break;
    }
    skCanvas->drawTextBlob(textBlob, x, y, fillPaint);
    this->notifyCanvasUpdated();
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::stroke()
  {
    if (TR_UNLIKELY(skCanvas == nullptr))
      return;

    // TODO: support path
    auto path = *currentPath;
    auto strokePaint = getStrokePaint();
    auto shadowPaint = getShadowPaint(strokePaint);
    if (shadowPaint != nullptr)
    {
      skCanvas->save();
      // TODO
      skCanvas->drawPath(path, *shadowPaint);
      skCanvas->restore();
      delete shadowPaint;
    }
    skCanvas->drawPath(path, strokePaint);
    this->notifyCanvasUpdated();
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::clearRect(float x, float y, float width, float height)
  {
    skPaint->setStyle(SkPaint::kFill_Style);
    skPaint->setBlendMode(SkBlendMode::kClear);
    if (skCanvas != nullptr)
    {
      skCanvas->drawRect(SkRect::MakeXYWH(x, y, width, height), *skPaint);
      this->notifyCanvasUpdated();
    }
    skPaint->setBlendMode(globalCompositeOperation);
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::setLineDash(const std::vector<float> &segments)
  {
    auto count = segments.size();
    if (count % 2 == 1)
    {
      lineDash.resize(count * 2);
      for (size_t i = 0; i < count; i++)
      {
        lineDash[i * 2] = segments[i];
        lineDash[i * 2 + 1] = segments[i];
      }
    }
    else
    {
      lineDash.resize(count);
      for (size_t i = 0; i < count; i++)
        lineDash[i] = segments[i];
    }
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::beginPath()
  {
    if (currentPath != nullptr)
      currentPath.reset();
    currentPath = std::make_shared<SkPath>();
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::closePath()
  {
    closeSkPath(currentPath);
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::moveTo(float x, float y)
  {
    if (currentPath != nullptr)
      currentPath->moveTo(x, y);
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::lineTo(float x, float y)
  {
    if (currentPath == nullptr)
      return;
    if (currentPath->isEmpty())
      currentPath->moveTo(x, y);
    currentPath->lineTo(x, y);
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y)
  {
    if (currentPath == nullptr)
      return;
    if (currentPath->isEmpty())
      currentPath->moveTo(cp1x, cp1y);
    currentPath->cubicTo(cp1x, cp1y, cp2x, cp2y, x, y);
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::quadraticCurveTo(float cpx, float cpy, float x, float y)
  {
    if (currentPath == nullptr)
      return;
    if (currentPath->isEmpty())
      currentPath->moveTo(cpx, cpy);
    currentPath->quadTo(cpx, cpy, x, y);
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::arc(float x, float y, float radius, float startAngle, float endAngle, bool ccw)
  {
    Path2D::Ellipse(currentPath, x, y, radius, radius, 0, startAngle, endAngle, ccw);
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::arcTo(float x1, float y1, float x2, float y2, float radius)
  {
    if (currentPath == nullptr)
      return;
    if (currentPath->isEmpty())
      currentPath->moveTo(x1, y1);
    Path2D::ApplyArcToTangent(currentPath, x1, y1, x2, y2, radius);
  }

  template <typename CanvasType>
  TextMetrics CanvasRenderingContext2D<CanvasType>::measureText(const std::string &text)
  {
    TextMetrics textMetrics;
    SkGlyphID ids[256];
    int glyphsCount = skFont->textToGlyphs(text.c_str(), text.size(), SkTextEncoding::kUTF8, ids, 256);

    SkScalar widths[glyphsCount];
    skFont->getWidths(ids, glyphsCount, widths);

    SkScalar totalWidth = 0;
    for (int i = 0; i < glyphsCount; i++)
      totalWidth += widths[i];
    textMetrics.width = totalWidth;

    SkFontMetrics metrics;
    skFont->getMetrics(&metrics);
    textMetrics.fontBoundingBoxAscent = metrics.fAscent;
    textMetrics.fontBoundingBoxDescent = metrics.fDescent;

    SkRect bounds;
    skFont->measureText(text.c_str(), text.size(), SkTextEncoding::kUTF8, &bounds);
    textMetrics.actualBoundingBoxLeft = bounds.left();
    textMetrics.actualBoundingBoxRight = bounds.right();
    textMetrics.actualBoundingBoxAscent = -bounds.top();
    textMetrics.actualBoundingBoxDescent = bounds.bottom();

    // Others
    textMetrics.emHeightAscent = metrics.fCapHeight;
    textMetrics.emHeightDescent = metrics.fXHeight;
    textMetrics.alphabeticBaseline = metrics.fAscent;
    textMetrics.hangingBaseline = metrics.fAscent - metrics.fCapHeight;
    textMetrics.ideographicBaseline = metrics.fAscent - metrics.fXHeight;
    return textMetrics;
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::transform(float a, float b, float c, float d, float e, float f)
  {
    if (TR_LIKELY(skCanvas != nullptr))
      skCanvas->concat(SkMatrix::MakeAll(a, b, e, c, d, f, 0, 0, 1));
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::setTransform(float a, float b, float c, float d, float e, float f)
  {
    if (TR_LIKELY(skCanvas != nullptr))
    {
      skCanvas->resetMatrix();
      skCanvas->concat(SkMatrix::MakeAll(a, b, e, c, d, f, 0, 0, 1));
    }
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::scale(float sx, float sy)
  {
    if (TR_LIKELY(skCanvas != nullptr))
      skCanvas->scale(sx, sy);
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::rotate(float angle)
  {
    if (TR_LIKELY(skCanvas != nullptr))
      skCanvas->rotate(angle);
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::translate(float tx, float ty)
  {
    if (TR_LIKELY(skCanvas != nullptr))
      skCanvas->translate(tx, ty);
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::drawImage(std::shared_ptr<ImageSource> image, SkRect dstRect, SkRect srcRect)
  {
    if (TR_UNLIKELY(skCanvas == nullptr) || image == nullptr)
      return;

    SkPaint imagePaint = getFillPaint();
    imagePaint.setColor(SK_ColorWHITE);
    sk_sp<SkImage> imageSource = image->makeSkImage();
    if (TR_UNLIKELY(imageSource == nullptr))
    {
      std::cerr << "Failed to load image to draw" << std::endl;
      return;
    }
    skCanvas->drawImageRect(imageSource, srcRect, dstRect,
                            SkSamplingOptions(), &imagePaint, SkCanvas::kFast_SrcRectConstraint);
    this->notifyCanvasUpdated();
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::drawImage(std::shared_ptr<ImageSource> image, float dx, float dy)
  {
    SkRect dstRect = SkRect::MakeXYWH(dx, dy, image->width(), image->height());
    SkRect srcRect = SkRect::MakeXYWH(0, 0, image->width(), image->height());
    drawImage(image, dstRect, srcRect);
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::drawImage(std::shared_ptr<ImageSource> image, float dx, float dy, float dWidth, float dHeight)
  {
    SkRect dstRect = SkRect::MakeXYWH(dx, dy, dWidth, dHeight);
    SkRect srcRect = SkRect::MakeXYWH(0, 0, image->width(), image->height());
    drawImage(image, dstRect, srcRect);
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::drawImage(std::shared_ptr<ImageSource> image,
                                                       float sx, float sy, float sWidth, float sHeight,
                                                       float dx, float dy, float dWidth, float dHeight)
  {
    SkRect dstRect = SkRect::MakeXYWH(dx, dy, dWidth, dHeight);
    SkRect srcRect = SkRect::MakeXYWH(sx, sy, sWidth, sHeight);
    drawImage(image, dstRect, srcRect);
  }

  inline bool isColorSpaceEqual(skcms_Matrix3x3 x, skcms_Matrix3x3 y)
  {
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        if (x.vals[i][j] != y.vals[i][j])
        {
          return false;
        }
      }
    }
    return true;
  }

  template <typename CanvasType>
  std::shared_ptr<ImageData> CanvasRenderingContext2D<CanvasType>::createImageData(float width, float height, const std::string &colorSpace)
  {
    return std::make_shared<ImageData>(width, height, colorSpace);
  }

  template <typename CanvasType>
  std::shared_ptr<ImageData> CanvasRenderingContext2D<CanvasType>::createImageData(std::shared_ptr<ImageData> otherImageData)
  {
    auto width = otherImageData->width();
    auto height = otherImageData->height();

    skcms_Matrix3x3 gamut;
    if (otherImageData->colorSpace()->toXYZD50(&gamut))
    {
      if (isColorSpaceEqual(gamut, SkNamedGamut::kDisplayP3))
        return createImageData(width, height, "display-p3");
      // TODO: support other color spaces? (e.g. AdobeRGB, ProPhotoRGB, etc.)
    }
    return createImageData(width, height);
  }

  template <typename CanvasType>
  std::shared_ptr<ImageData> CanvasRenderingContext2D<CanvasType>::getImageData(float x, float y, float width, float height)
  {
    if (TR_UNLIKELY(skCanvas == nullptr))
      return nullptr;

    SkImageInfo dstImageInfo = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    uint8_t *dstPixels = new uint8_t[dstImageInfo.computeMinByteSize()];
    if (!skCanvas->readPixels(dstImageInfo, dstPixels, dstImageInfo.minRowBytes(), x, y))
      return nullptr;

    std::vector<char> dataArray(dstPixels, dstPixels + dstImageInfo.computeMinByteSize());
    delete[] dstPixels;
    return std::make_shared<ImageData>(dataArray, width, height);
  }

  template <typename CanvasType>
  bool CanvasRenderingContext2D<CanvasType>::putImageData(std::shared_ptr<ImageSource> image, float dx, float dy)
  {
    if (TR_UNLIKELY(skCanvas == nullptr))
      return false;

    auto bitmap = image->makeSkBitmap();
    if (TR_UNLIKELY(bitmap == nullptr))
      return false;

    bool r = skCanvas->writePixels(*bitmap, dx, dy);
    this->notifyCanvasUpdated();
    return r;
  }

  template <typename CanvasType>
  bool CanvasRenderingContext2D<CanvasType>::putImageData(std::shared_ptr<ImageSource> image,
                                                          float dx, float dy,
                                                          float dirtyX, float dirtyY, float dirtyWidth, float dirtyHeight)
  {
    if (TR_UNLIKELY(skCanvas == nullptr))
      return false;

    if (dirtyWidth < 0)
    {
      dirtyX += dirtyWidth;
      dirtyWidth = abs(dirtyWidth);
    }
    if (dirtyHeight < 0)
    {
      dirtyY += dirtyHeight;
      dirtyHeight = abs(dirtyHeight);
    }
    if (dirtyX < 0)
    {
      dirtyWidth += dirtyX;
      dirtyX = 0;
    }
    if (dirtyY < 0)
    {
      dirtyHeight += dirtyY;
      dirtyY = 0;
    }

    if (dirtyWidth <= 0 || dirtyHeight <= 0)
      return false;

    SkRect srcRect = SkRect::MakeXYWH(dirtyX, dirtyY, dirtyWidth, dirtyHeight);
    SkRect dstRect = SkRect::MakeXYWH(dx + dirtyX, dy + dirtyY, dirtyWidth, dirtyHeight);

    SkMatrix invertedCtm;
    if (currentTransform.invert(&invertedCtm))
    {
      skCanvas->save();
      skCanvas->concat(invertedCtm);
      skCanvas->drawImageRect(image->makeSkImage(), srcRect, dstRect,
                              SkSamplingOptions(), nullptr, SkCanvas::kFast_SrcRectConstraint);
      skCanvas->restore();
      this->notifyCanvasUpdated();
      return true;
    }
    else
    {
      return false;
    }
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::save()
  {
    if (TR_UNLIKELY(skCanvas == nullptr))
      return;

    // Save the current state of the canvas
    // This includes the current transformation matrix, clip, and paint state.
    // NOTE(yorkie): SkCanvas::save() does not save the paint state, so we need to handle it manually.
    skCanvas->save();
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::restore()
  {
    if (TR_UNLIKELY(skCanvas == nullptr))
      return;

    // Restore the last saved state of the canvas
    // This includes the current transformation matrix, clip, and paint state.
    skCanvas->restore();
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::reset(sk_sp<SkSurface> skSurface)
  {
    if (skSurface == nullptr)
    {
      skCanvas = nullptr;
    }
    else
    {
      skCanvas = skSurface->getCanvas();
      skCanvas->drawColor(SK_ColorWHITE);
      // TODO(yorkie): should redraw last content?
    }
  }

  template <typename CanvasType>
  SkPaint CanvasRenderingContext2D<CanvasType>::getFillPaint()
  {
    SkPaint paint(*skPaint);
    paint.setStyle(SkPaint::kFill_Style);
    paint.setColor(fillStyle);
    // TODO: pattern, gradient
    return paint;
  }

  template <typename CanvasType>
  SkPaint CanvasRenderingContext2D<CanvasType>::getStrokePaint()
  {
    SkPaint paint = *skPaint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setColor(strokeStyle);
    // TODO
    paint.setStrokeWidth(strokeWidth);

    if (lineDash.size())
    {
      auto effect = SkDashPathEffect::Make(lineDash.data(), lineDash.size(), 0);
      paint.setPathEffect(effect);
    }
    return paint;
  }

  template <typename CanvasType>
  SkPaint *CanvasRenderingContext2D<CanvasType>::getShadowPaint(SkPaint &basePaint)
  {
    return nullptr;
  }

  template <typename CanvasType>
  void CanvasRenderingContext2D<CanvasType>::closeSkPath(std::shared_ptr<SkPath> path)
  {
    if (path == nullptr || path->isEmpty())
      return;
    SkRect bounds = path->getBounds();
    if (bounds.width() || bounds.height())
      path->close();
  }

  template <typename CanvasType>
  bool CanvasRenderingContext2D<CanvasType>::ellipseToSkPath(std::shared_ptr<SkPath> path,
                                                             float x, float y,
                                                             float radiusX, float radiusY, float rotation,
                                                             float startAngle, float endAngle, bool ccw)
  {
    if (radiusX < 0 || radiusY < 0)
      return false;

    auto tao = 2 * M_PI;
    auto newStartAngle = fmod(startAngle, tao);
    if (newStartAngle < 0)
      newStartAngle += tao;
    auto delta = newStartAngle - startAngle;
    startAngle = newStartAngle;
    endAngle += delta;

    if (!ccw && (endAngle - startAngle) >= tao)
      endAngle = startAngle + tao;
    else if (ccw && (startAngle - endAngle) >= tao)
      endAngle = startAngle - tao;
    else if (!ccw && startAngle > endAngle)
      endAngle = startAngle + (tao - fmod(startAngle - endAngle, tao));
    else if (ccw && startAngle < endAngle)
      endAngle = startAngle - (tao - fmod(endAngle - startAngle, tao));

    // TODO
    return true;
  }
}
