#include <vector>
#include <client/macros.h>
#include "./web_content.hpp"

namespace builtin_scene
{
  WebContent::WebContent(SkCanvas *canvas, std::string name, client_cssom::CSSStyleDeclaration &style)
      : canvas_(canvas),
        name_(name),
        style_(style),
        lastLayout_(std::nullopt)
  {
    SkPaint transparent;
#ifdef TR_CLIENT_WEB_CONTENT_DEBUG_TEXT
    transparent.setColor(SK_ColorGRAY);
#else
    transparent.setColor(SK_ColorTRANSPARENT);
#endif

    SkPaint textPaint;
    textPaint.setAntiAlias(true);
    textPaint.setColor(SK_ColorWHITE);

    // Init text style
    textStyle.setBackgroundColor(transparent);
    textStyle.setForegroundColor(textPaint);
    textStyle.setFontSize(20.0f);
    textStyle.setFontFamilies({SkString("PingFang SC"),
                               SkString("Arial"),
                               SkString("sans-serif")});

    // Init paragraph style
    paragraphStyle.setTextAlign(skia::textlayout::TextAlign::kLeft);
    paragraphStyle.setTextDirection(skia::textlayout::TextDirection::kLtr);
    paragraphStyle.setTextStyle(textStyle);
  }

  void WebContent::setStyle(const client_cssom::CSSStyleDeclaration &style)
  {
    style_ = style;
    setDirty(true); // Mark the content as dirty if setting a new style.

    // Update the text style
    bool isTextStyleChanged = false;
    {
      if (style_.hasProperty("font-family"))
      {
        auto fonts = crates::css::parseFontFamily(style_.getPropertyValue("font-family"));
        if (fonts.size() >= 1)
        {
          vector<SkString> skFonts;
          for (auto &font : fonts)
            skFonts.push_back(SkString(font));
          textStyle.setFontFamilies(skFonts);
          isTextStyleChanged = true;
        }
      }
      if (style_.hasProperty("font-size"))
      {
        textStyle.setFontSize(style_.getPropertyValueAs<float>("font-size"));
        isTextStyleChanged = true;
      }
      if (style_.hasProperty("font-weight"))
      {
        SkFontStyle fontStyle = textStyle.getFontStyle();
        auto fontWeight = style_.getPropertyValueAs<client_cssom::types::FontWeight>("font-weight");
        textStyle.setFontStyle(SkFontStyle(fontWeight.value(), fontStyle.width(), fontStyle.slant()));
        isTextStyleChanged = true;
      }
      if (style_.hasProperty("font-style"))
      {
        SkFontStyle fontStyle = textStyle.getFontStyle();
        auto fontSlant = style_.getPropertyValueAs<client_cssom::types::FontStyle>("font-style");
        textStyle.setFontStyle(SkFontStyle(fontStyle.weight(), fontStyle.width(), fontSlant));
        isTextStyleChanged = true;
      }
    }

    // Update Paragraph styles
    {
      if (style_.hasProperty("color"))
      {
        SkPaint textPaint;
        textPaint.setAntiAlias(true);
        textPaint.setColor(style_.getPropertyValueAs<client_cssom::types::Color>("color"));
        textStyle.setForegroundColor(textPaint);
        isTextStyleChanged = true;
      }
      if (style_.hasProperty("text-align"))
        paragraphStyle.setTextAlign(style_.getPropertyValueAs<client_cssom::types::TextAlign>("text-align"));
      if (style_.hasProperty("direction"))
        paragraphStyle.setTextDirection(style_.getPropertyValueAs<client_cssom::types::Direction>("direction"));
    }

    // Update the text style if changed
    if (isTextStyleChanged)
      paragraphStyle.setTextStyle(textStyle);
  }
}
