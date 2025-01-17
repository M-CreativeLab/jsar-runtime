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
    transparent.setColor(SK_ColorTRANSPARENT);

    SkPaint textPaint;
    textPaint.setAntiAlias(true);
    textPaint.setColor(SK_ColorWHITE);

    // Init text style
    textStyle.setBackgroundColor(transparent);
    textStyle.setForegroundColor(textPaint);
    textStyle.setFontSize(30.0f);
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

    // Update the text style
    bool isTextStyleChanged = false;
    {
      if (style_.hasProperty("font-size"))
      {
        textStyle.setFontSize(style_.getPropertyValueAs<float>("font-size"));
        isTextStyleChanged = true;
      }
      if (style_.hasProperty("font-family"))
      {
        // TODO: Parse the font family.
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
