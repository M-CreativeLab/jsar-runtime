#include <cmath>
#include <vector>
#include <skia/include/core/SkImageInfo.h>
#include <client/macros.h>

#include "./web_content.hpp"

namespace builtin_scene
{
  using namespace std;

  WebContentTextStyle::WebContentTextStyle()
      : color(SK_ColorBLACK),
#ifdef TR_CLIENT_WEB_CONTENT_DEBUG_TEXT
        backgroundColor(SK_ColorGRAY),
#else
        backgroundColor(SK_ColorTRANSPARENT),
#endif
        decoration(0),
        decorationThickness(0.0f),
        decorationColor(SK_ColorBLACK),
        fontStyle({SkFontStyle::kUpright_Slant, SkFontStyle::kNormal_Weight, SkFontStyle::kNormal_Width}),
#ifdef __APPLE__
        fontFamilies({SkString("PingFang SC"), SkString("sans-serif")}),
#elif __ANDROID__
        fontFamilies({SkString("Noto Sans"), SkString("sans-serif")}),
#else
        fontFamilies({SkString("sans-serif")}),
#endif
        fontSize(20.0f),
        letterSpacing(std::nullopt),
        wordSpacing(std::nullopt)
  {
  }

  WebContentStyle::WebContentStyle()
      : disableHinting(false),
        maxLines(0),
        textAlign(skia::textlayout::TextAlign::kLeft),
        textDirection(skia::textlayout::TextDirection::kLtr),
        textHeightBehavior(skia::textlayout::TextHeightBehavior::kAll),
        textStyle(),
        lineHeight(1.5f),
        useFixedLineHeight(false),
        halfLeading(true),
        leading(0.0f),
        strutEnabled(true),
        forceStrutHeight(true),
        applyRoundingHack(false)
  {
  }

  WebContent::WebContent(std::string name, float initialWidth, float initialHeight)
      : name_(name),
        last_fragment_(std::nullopt),
        content_style_(),
        background_color_(1.0f, 1.0f, 1.0f, 0.0f),
        device_pixel_ratio_(1.0f)
  {
    resetSkSurface(initialWidth, initialHeight);
  }

  // Compute the size in pixels by the given size and device pixel ratio. This function also rounds the size to the
  // nearest integer to avoid the floating point precision issue.
  inline int computeSize(float size, float devicePixelRatio, int pad)
  {
    return static_cast<int>(round(size * devicePixelRatio));
  }

  bool WebContent::resetSkSurface(float w, float h)
  {
    assert(device_pixel_ratio_ > 0 && "The device pixel ratio must be valid.");

    if (TR_UNLIKELY(w <= 0 || h <= 0)) // Skip if size is invalid.
      return false;
    if (!needsResize(w, h)) // Skip if size is unchanged.
      return false;

    // TODO: use Skia Genesh(GPU) to increase the performance.
    SkImageInfo imageInfo = SkImageInfo::MakeN32Premul(computeSize(w, device_pixel_ratio_, texture_pad_),
                                                       computeSize(h, device_pixel_ratio_, texture_pad_));
    if (surface_ != nullptr)
    {
      auto newSurface = surface_->makeSurface(imageInfo);
      surface_.reset();
      surface_ = newSurface;
    }
    else
    {
      surface_ = SkSurfaces::Raster(imageInfo);
    }

    assert(surface_ != nullptr && "The new surface must be valid.");
    assert(surface_->width() == imageInfo.width() &&
           surface_->height() == imageInfo.height() &&
           "The surface size must be valid.");

    setDirty(true);
    return true;
  }

  SkCanvas *WebContent::canvas() const
  {
    if (TR_UNLIKELY(surface_ == nullptr))
      return nullptr;

    SkCanvas *canvas = surface_->getCanvas();
    canvas->setMatrix(SkMatrix::Translate(texture_pad_, texture_pad_));
    // TODO(yorkie): support scaling the canvas.
    return canvas;
  }

  void WebContent::setStyle(const client_cssom::ComputedStyle &style, std::shared_ptr<WebContent> parent)
  {
    style_ = style;

    // Update the text style
    {
      if (style_.hasProperty("font-family"))
      {
        auto &fonts = style_.fonts();
        if (fonts.size() >= 1)
        {
          vector<SkString> skFonts;
          for (auto &font : fonts)
            skFonts.push_back(SkString(font));
          content_style_.textStyle.fontFamilies = skFonts;
        }
      }
      if (style_.hasProperty("font-size"))
      {
        const auto &fontSize = style_.fontSize();
        content_style_.textStyle.fontSize = fontSize.computedSize().px();
      }
      if (style_.hasProperty("font-weight"))
      {
        auto fontWeight = style_.fontWeight();
        content_style_.textStyle.fontStyle.weight = SkFontStyle::Weight(fontWeight.value());
      }
      if (style_.hasProperty("font-style"))
        content_style_.textStyle.fontStyle.slant = style_.fontStyle();
    }

    // Update Paragraph styles
    {
      if (style_.hasProperty("color"))
      {
        // TODO(yorkie): support current color.
        auto current_color = SK_ColorBLACK;
        content_style_.textStyle.color = style_.color().resolveToAbsoluteColor(current_color);
      }
      if (style_.hasProperty("text-align"))
        content_style_.textAlign = style_.textAlign();
      if (style_.hasProperty("direction"))
        content_style_.textDirection = style_.textDirection();

      // Line height
      if (style_.hasProperty("line-height"))
      {
        const auto &lineHeight = style_.lineHeight();
        if (lineHeight.isLength())
        {
          content_style_.useFixedLineHeight = true;
          content_style_.lineHeight = lineHeight.getLength().px();
        }
        else if (lineHeight.isNumber())
        {
          content_style_.useFixedLineHeight = false;
          content_style_.lineHeight = lineHeight.getNumber().value;
        }
        else
        {
          content_style_.useFixedLineHeight = false;
          content_style_.lineHeight = 1.2f;
        }
      }
    }

    // Mark the content as dirty if setting a new style.
    setDirty(true);
  }

  // TODO(yorkie): consider the change of the device pixel ratio.
  bool WebContent::needsResize(float w, float h) const
  {
    assert(w > 0 && h > 0);
    if (TR_UNLIKELY(surface_ == nullptr))
      return true;

    return surface_->width() != computeSize(w, device_pixel_ratio_, texture_pad_) ||
           surface_->height() != computeSize(h, device_pixel_ratio_, texture_pad_);
  }

  shared_ptr<Texture> WebContent::resizeOrInitTexture(TextureAtlas &textureAtlas)
  {
    if (!is_texture_using_)
    {
      // Remove the texture from atlas if it's not used.
      if (texture_ != nullptr)
      {
        textureAtlas.removeTexture(*texture_);
        texture_ = nullptr;
      }
      return nullptr;
    }

    float w = physicalWidth();
    float h = physicalHeight();

    if (texture_ == nullptr)
      texture_ = textureAtlas.addTexture(w, h, true);
    else
      texture_ = textureAtlas.resizeTexture(texture_, w, h, true);

    assert(texture_ != nullptr && "The texture must be valid.");
    return texture_;
  }

  skia::textlayout::TextStyle WebContent::textStyle() const
  {
    const WebContentTextStyle &sourceTextStyle = content_style_.textStyle;
    skia::textlayout::TextStyle newTextStyle;

    newTextStyle.setColor(sourceTextStyle.color);
    if (sourceTextStyle.foregroundColor.has_value())
    {
      SkPaint foregroundPaint;
      foregroundPaint.setColor(sourceTextStyle.foregroundColor.value());
      newTextStyle.setForegroundColor(foregroundPaint);
    }
    if (sourceTextStyle.backgroundColor.has_value())
    {
      SkPaint backgroundPaint;
      backgroundPaint.setAntiAlias(true);
      backgroundPaint.setColor(sourceTextStyle.backgroundColor.value());
      newTextStyle.setBackgroundColor(backgroundPaint);
    }

    newTextStyle.setFontSize(sourceTextStyle.fontSize);
    if (sourceTextStyle.letterSpacing.has_value())
      newTextStyle.setLetterSpacing(sourceTextStyle.letterSpacing.value());
    if (sourceTextStyle.wordSpacing.has_value())
      newTextStyle.setWordSpacing(sourceTextStyle.wordSpacing.value());

    newTextStyle.setHalfLeading(content_style_.halfLeading);
    newTextStyle.setFontFamilies(sourceTextStyle.fontFamilies);
    newTextStyle.setFontStyle(SkFontStyle(sourceTextStyle.fontStyle.weight,
                                          sourceTextStyle.fontStyle.width,
                                          sourceTextStyle.fontStyle.slant));
    return newTextStyle;
  }

  skia::textlayout::StrutStyle WebContent::structStyle() const
  {
    skia::textlayout::StrutStyle newStrutStyle;
    newStrutStyle.setFontFamilies(content_style_.textStyle.fontFamilies);

    SkFontStyle fontStyle = SkFontStyle(content_style_.textStyle.fontStyle.weight,
                                        content_style_.textStyle.fontStyle.width,
                                        content_style_.textStyle.fontStyle.slant);
    newStrutStyle.setFontStyle(fontStyle);

    auto &textStyle = content_style_.textStyle;
    newStrutStyle.setFontSize(textStyle.fontSize);
    newStrutStyle.setHalfLeading(content_style_.halfLeading);

    // Reconfigure the font size based on the line height settings.
    if (content_style_.useFixedLineHeight)
      newStrutStyle.setFontSize(content_style_.lineHeight);
    else
      newStrutStyle.setFontSize(textStyle.fontSize * content_style_.lineHeight);

    newStrutStyle.setStrutEnabled(true);
    newStrutStyle.setForceStrutHeight(true);
    return newStrutStyle;
  }

  skia::textlayout::ParagraphStyle WebContent::paragraphStyle() const
  {
    skia::textlayout::ParagraphStyle newParagraphStyle;
    if (content_style_.disableHinting)
      newParagraphStyle.turnHintingOff();

    newParagraphStyle.setTextAlign(content_style_.textAlign);
    newParagraphStyle.setTextDirection(content_style_.textDirection);
    newParagraphStyle.setTextStyle(textStyle());
    newParagraphStyle.setStrutStyle(structStyle());

    if (content_style_.maxLines > 0)
      newParagraphStyle.setMaxLines(content_style_.maxLines);

    newParagraphStyle.setApplyRoundingHack(content_style_.applyRoundingHack);
    newParagraphStyle.setTextHeightBehavior(content_style_.textHeightBehavior);
    return newParagraphStyle;
  }
}
