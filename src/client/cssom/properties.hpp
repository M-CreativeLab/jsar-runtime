#pragma once

#include <algorithm>
#include <variant>
#include <string>
#include <optional>
#include <functional>
#include <iostream>
#include <stdint.h>

namespace client_cssom
{
#define SHORTHAND_PROPS_MAP(XX)                   \
  XX(All, "all")                                  \
  /* Box Model */                                 \
  XX(Margin, "margin")                            \
  XX(Padding, "padding")                          \
  XX(Border, "border")                            \
  XX(BorderTop, "border-top")                     \
  XX(BorderRight, "border-right")                 \
  XX(BorderBottom, "border-bottom")               \
  XX(BorderLeft, "border-left")                   \
  XX(BorderWidth, "border-width")                 \
  XX(BorderStyle, "border-style")                 \
  XX(BorderColor, "border-color")                 \
  XX(BorderRadius, "border-radius")               \
  XX(Outline, "outline")                          \
  /* Background */                                \
  XX(Background, "background")                    \
  XX(BackgroundPosition, "background-position")   \
  XX(BackgroundRepeat, "background-repeat")       \
  XX(BackgroundSize, "background-size")           \
  /* Flexbox */                                   \
  XX(Flex, "flex")                                \
  XX(FlexFlow, "flex-flow")                       \
  /* Grid */                                      \
  XX(Grid, "grid")                                \
  XX(GridTemplate, "grid-template")               \
  XX(GridArea, "grid-area")                       \
  XX(GridColumn, "grid-column")                   \
  XX(GridRow, "grid-row")                         \
  XX(Gap, "gap")                                  \
  /* Typography */                                \
  XX(Font, "font")                                \
  XX(TextDecoration, "text-decoration")           \
  XX(ListStyle, "list-style")                     \
  /* Transforms & Transitions */                  \
  XX(Transition, "transition")                    \
  XX(Animation, "animation")                      \
  XX(Transform, "transform")                      \
  /* Logical Properties */                        \
  XX(Inset, "inset")                              \
  XX(MarginBlock, "margin-block")                 \
  XX(MarginInline, "margin-inline")               \
  XX(PaddingBlock, "padding-block")               \
  XX(PaddingInline, "padding-inline")             \
  XX(BorderBlock, "border-block")                 \
  XX(BorderInline, "border-inline")               \
  /* Multi-column */                              \
  XX(Columns, "columns")                          \
  XX(ColumnRule, "column-rule")                   \
  /* Scroll */                                    \
  XX(ScrollMargin, "scroll-margin")               \
  XX(ScrollPadding, "scroll-padding")             \
  /* CSS Box Alignment */                         \
  XX(PlaceContent, "place-content")               \
  XX(PlaceItems, "place-items")                   \
  XX(PlaceSelf, "place-self")                     \
  /* Deprecated/WebKit */                         \
  XX(WebkitTextStroke, "-webkit-text-stroke")     \
  XX(WebkitBorderBefore, "-webkit-border-before") \
  XX(WebkitBorderAfter, "-webkit-border-after")   \
  XX(WebkitBorderStart, "-webkit-border-start")   \
  XX(WebkitBorderEnd, "-webkit-border-end")

#define LONGHAND_PROPS_MAP(XX)                               \
  /* Box Model */                                            \
  XX(MarginTop, "margin-top")                                \
  XX(MarginRight, "margin-right")                            \
  XX(MarginBottom, "margin-bottom")                          \
  XX(MarginLeft, "margin-left")                              \
  XX(PaddingTop, "padding-top")                              \
  XX(PaddingRight, "padding-right")                          \
  XX(PaddingBottom, "padding-bottom")                        \
  XX(PaddingLeft, "padding-left")                            \
  XX(BorderTopWidth, "border-top-width")                     \
  XX(BorderRightWidth, "border-right-width")                 \
  XX(BorderBottomWidth, "border-bottom-width")               \
  XX(BorderLeftWidth, "border-left-width")                   \
  XX(BorderTopStyle, "border-top-style")                     \
  XX(BorderRightStyle, "border-right-style")                 \
  XX(BorderBottomStyle, "border-bottom-style")               \
  XX(BorderLeftStyle, "border-left-style")                   \
  XX(BorderTopColor, "border-top-color")                     \
  XX(BorderRightColor, "border-right-color")                 \
  XX(BorderBottomColor, "border-bottom-color")               \
  XX(BorderLeftColor, "border-left-color")                   \
  XX(BorderTopLeftRadius, "border-top-left-radius")          \
  XX(BorderTopRightRadius, "border-top-right-radius")        \
  XX(BorderBottomLeftRadius, "border-bottom-left-radius")    \
  XX(BorderBottomRightRadius, "border-bottom-right-radius")  \
  XX(Width, "width")                                         \
  XX(Height, "height")                                       \
  XX(MinWidth, "min-width")                                  \
  XX(MinHeight, "min-height")                                \
  XX(MaxWidth, "max-width")                                  \
  XX(MaxHeight, "max-height")                                \
  XX(BoxSizing, "box-sizing")                                \
  /* Background & Borders */                                 \
  XX(BackgroundColor, "background-color")                    \
  XX(BackgroundImage, "background-image")                    \
  XX(BackgroundPositionX, "background-position-x")           \
  XX(BackgroundPositionY, "background-position-y")           \
  XX(BackgroundSize, "background-size")                      \
  XX(BackgroundRepeatX, "background-repeat-x")               \
  XX(BackgroundRepeatY, "background-repeat-y")               \
  XX(BackgroundOrigin, "background-origin")                  \
  XX(BackgroundClip, "background-clip")                      \
  XX(BackgroundAttachment, "background-attachment")          \
  XX(BackgroundBlendMode, "background-blend-mode")           \
  XX(BorderImageSource, "border-image-source")               \
  XX(BorderImageSlice, "border-image-slice")                 \
  XX(BorderImageWidth, "border-image-width")                 \
  XX(BorderImageOutset, "border-image-outset")               \
  XX(BorderImageRepeat, "border-image-repeat")               \
  /* Flexbox */                                              \
  XX(FlexGrow, "flex-grow")                                  \
  XX(FlexShrink, "flex-shrink")                              \
  XX(FlexBasis, "flex-basis")                                \
  XX(FlexDirection, "flex-direction")                        \
  XX(FlexWrap, "flex-wrap")                                  \
  XX(Order, "order")                                         \
  XX(AlignContent, "align-content")                          \
  XX(AlignItems, "align-items")                              \
  XX(AlignSelf, "align-self")                                \
  XX(JustifyContent, "justify-content")                      \
  XX(JustifyItems, "justify-items")                          \
  XX(JustifySelf, "justify-self")                            \
  /* Grid */                                                 \
  XX(GridColumnStart, "grid-column-start")                   \
  XX(GridColumnEnd, "grid-column-end")                       \
  XX(GridRowStart, "grid-row-start")                         \
  XX(GridRowEnd, "grid-row-end")                             \
  XX(GridTemplateColumns, "grid-template-columns")           \
  XX(GridTemplateRows, "grid-template-rows")                 \
  XX(GridTemplateAreas, "grid-template-areas")               \
  XX(GridAutoColumns, "grid-auto-columns")                   \
  XX(GridAutoRows, "grid-auto-rows")                         \
  XX(GridAutoFlow, "grid-auto-flow")                         \
  XX(Gap, "gap")                                             \
  XX(RowGap, "row-gap")                                      \
  XX(ColumnGap, "column-gap")                                \
  /* Typography */                                           \
  XX(FontFamily, "font-family")                              \
  XX(FontSize, "font-size")                                  \
  XX(FontStyle, "font-style")                                \
  XX(FontVariant, "font-variant")                            \
  XX(FontWeight, "font-weight")                              \
  XX(LineHeight, "line-height")                              \
  XX(LetterSpacing, "letter-spacing")                        \
  XX(WordSpacing, "word-spacing")                            \
  XX(TextAlign, "text-align")                                \
  XX(TextDecorationLine, "text-decoration-line")             \
  XX(TextDecorationStyle, "text-decoration-style")           \
  XX(TextDecorationColor, "text-decoration-color")           \
  XX(TextDecorationThickness, "text-decoration-thickness")   \
  XX(TextUnderlineOffset, "text-underline-offset")           \
  XX(TextUnderlinePosition, "text-underline-position")       \
  XX(TextTransform, "text-transform")                        \
  XX(TextIndent, "text-indent")                              \
  XX(VerticalAlign, "vertical-align")                        \
  XX(WhiteSpace, "white-space")                              \
  XX(TabSize, "tab-size")                                    \
  XX(WordBreak, "word-break")                                \
  XX(OverflowWrap, "overflow-wrap")                          \
  XX(Hyphens, "hyphens")                                     \
  /* Visual Effects */                                       \
  XX(Opacity, "opacity")                                     \
  XX(BoxShadow, "box-shadow")                                \
  XX(Filter, "filter")                                       \
  XX(BackdropFilter, "backdrop-filter")                      \
  XX(MixBlendMode, "mix-blend-mode")                         \
  XX(ClipPath, "clip-path")                                  \
  XX(MaskImage, "mask-image")                                \
  XX(MaskPosition, "mask-position")                          \
  XX(MaskSize, "mask-size")                                  \
  XX(MaskRepeat, "mask-repeat")                              \
  XX(MaskOrigin, "mask-origin")                              \
  XX(MaskClip, "mask-clip")                                  \
  XX(MaskComposite, "mask-composite")                        \
  /* Transforms */                                           \
  XX(Transform, "transform")                                 \
  XX(TransformOrigin, "transform-origin")                    \
  XX(TransformStyle, "transform-style")                      \
  XX(Perspective, "perspective")                             \
  XX(PerspectiveOrigin, "perspective-origin")                \
  XX(BackfaceVisibility, "backface-visibility")              \
  XX(Translate, "translate")                                 \
  XX(Rotate, "rotate")                                       \
  XX(Scale, "scale")                                         \
  /* Transitions & Animations */                             \
  XX(TransitionProperty, "transition-property")              \
  XX(TransitionDuration, "transition-duration")              \
  XX(TransitionTimingFunction, "transition-timing-function") \
  XX(TransitionDelay, "transition-delay")                    \
  XX(AnimationName, "animation-name")                        \
  XX(AnimationDuration, "animation-duration")                \
  XX(AnimationTimingFunction, "animation-timing-function")   \
  XX(AnimationDelay, "animation-delay")                      \
  XX(AnimationIterationCount, "animation-iteration-count")   \
  XX(AnimationDirection, "animation-direction")              \
  XX(AnimationFillMode, "animation-fill-mode")               \
  XX(AnimationPlayState, "animation-play-state")             \
  /* Logical Properties */                                   \
  XX(InlineSize, "inline-size")                              \
  XX(BlockSize, "block-size")                                \
  XX(MarginInlineStart, "margin-inline-start")               \
  XX(MarginInlineEnd, "margin-inline-end")                   \
  XX(MarginBlockStart, "margin-block-start")                 \
  XX(MarginBlockEnd, "margin-block-end")                     \
  XX(PaddingInlineStart, "padding-inline-start")             \
  XX(PaddingInlineEnd, "padding-inline-end")                 \
  XX(PaddingBlockStart, "padding-block-start")               \
  XX(PaddingBlockEnd, "padding-block-end")                   \
  XX(BorderInlineStartWidth, "border-inline-start-width")    \
  XX(BorderInlineEndWidth, "border-inline-end-width")        \
  XX(BorderBlockStartWidth, "border-block-start-width")      \
  XX(BorderBlockEndWidth, "border-block-end-width")          \
  /* Scroll Snap */                                          \
  XX(ScrollMarginTop, "scroll-margin-top")                   \
  XX(ScrollMarginRight, "scroll-margin-right")               \
  XX(ScrollMarginBottom, "scroll-margin-bottom")             \
  XX(ScrollMarginLeft, "scroll-margin-left")                 \
  XX(ScrollPaddingTop, "scroll-padding-top")                 \
  XX(ScrollPaddingRight, "scroll-padding-right")             \
  XX(ScrollPaddingBottom, "scroll-padding-bottom")           \
  XX(ScrollPaddingLeft, "scroll-padding-left")               \
  XX(ScrollSnapType, "scroll-snap-type")                     \
  XX(ScrollSnapAlign, "scroll-snap-align")                   \
  XX(ScrollSnapStop, "scroll-snap-stop")                     \
  /* UI & Interaction */                                     \
  XX(Cursor, "cursor")                                       \
  XX(UserSelect, "user-select")                              \
  XX(PointerEvents, "pointer-events")                        \
  XX(Resize, "resize")                                       \
  XX(OutlineWidth, "outline-width")                          \
  XX(OutlineStyle, "outline-style")                          \
  XX(OutlineColor, "outline-color")                          \
  XX(OutlineOffset, "outline-offset")                        \
  XX(CaretColor, "caret-color")                              \
  XX(TextOverflow, "text-overflow")                          \
  XX(OverflowX, "overflow-x")                                \
  XX(OverflowY, "overflow-y")                                \
  XX(OverscrollBehaviorX, "overscroll-behavior-x")           \
  XX(OverscrollBehaviorY, "overscroll-behavior-y")           \
  XX(TouchAction, "touch-action")                            \
  /* Paged Media */                                          \
  XX(PageBreakBefore, "page-break-before")                   \
  XX(PageBreakAfter, "page-break-after")                     \
  XX(PageBreakInside, "page-break-inside")                   \
  XX(Orphans, "orphans")                                     \
  XX(Widows, "widows")                                       \
  /* SVG */                                                  \
  XX(Fill, "fill")                                           \
  XX(FillOpacity, "fill-opacity")                            \
  XX(FillRule, "fill-rule")                                  \
  XX(Stroke, "stroke")                                       \
  XX(StrokeOpacity, "stroke-opacity")                        \
  XX(StrokeWidth, "stroke-width")                            \
  XX(StrokeDasharray, "stroke-dasharray")                    \
  XX(StrokeDashoffset, "stroke-dashoffset")                  \
  XX(StrokeLinecap, "stroke-linecap")                        \
  XX(StrokeLinejoin, "stroke-linejoin")                      \
  XX(StrokeMiterlimit, "stroke-miterlimit")                  \
  /* Deprecated/WebKit */                                    \
  XX(WebkitAppearance, "-webkit-appearance")                 \
  XX(WebkitOverflowScrolling, "-webkit-overflow-scrolling")  \
  XX(WebkitTapHighlightColor, "-webkit-tap-highlight-color") \
  XX(WebkitTextFillColor, "-webkit-text-fill-color")         \
  XX(WebkitTextStrokeWidth, "-webkit-text-stroke-width")     \
  XX(WebkitTextStrokeColor, "-webkit-text-stroke-color")

  enum class ShorthandId : uint16_t
  {
#define XX(ID, _) k##ID,
    SHORTHAND_PROPS_MAP(XX)
#undef XX
  };

  inline std::string to_string(const ShorthandId &id)
  {
    switch (id)
    {
#define XX(ID, STR)        \
  case ShorthandId::k##ID: \
    return STR;
      SHORTHAND_PROPS_MAP(XX)
#undef XX
    }
    return "Unknown ShorthandId";
  }

  inline std::optional<ShorthandId> parse_shorthand(const std::string &str)
  {
#define XX(ID, STR) \
  if (str == STR)   \
    return ShorthandId::k##ID;
    SHORTHAND_PROPS_MAP(XX)
#undef XX
    return std::nullopt; // Return nullopt if no match found
  }

  enum class LonghandId : uint16_t
  {
#define XX(ID, _) k##ID,
    LONGHAND_PROPS_MAP(XX)
#undef XX
  };

  inline std::string to_string(const LonghandId &id)
  {
    switch (id)
    {
#define XX(ID, STR)       \
  case LonghandId::k##ID: \
    return STR;
      LONGHAND_PROPS_MAP(XX)
#undef XX
    }
    return "Unknown LonghandId";
  }

  inline std::optional<LonghandId> parse_longhand(const std::string &str)
  {
#define XX(ID, STR) \
  if (str == STR)   \
    return LonghandId::k##ID;
    LONGHAND_PROPS_MAP(XX)
#undef XX
    return std::nullopt; // Return nullopt if no match found
  }
#undef SHORTHAND_PROPS_MAP
#undef LONGHAND_PROPS_MAP

  class NonCustomPropertyId
  {
  public:
    static NonCustomPropertyId FromShorthand(ShorthandId shorthand_id)
    {
      return NonCustomPropertyId(shorthand_id);
    }
    static NonCustomPropertyId FromLonghand(LonghandId longhand_id)
    {
      return NonCustomPropertyId(longhand_id);
    }
    static std::optional<NonCustomPropertyId> Parse(const std::string &str)
    {
      if (auto shorthand_id = parse_shorthand(str))
        return FromShorthand(*shorthand_id);
      if (auto longhand_id = parse_longhand(str))
        return FromLonghand(*longhand_id);
      return std::nullopt; // Return nullopt if no match found
    }

  private:
    NonCustomPropertyId(ShorthandId shorthand_id)
        : bit_(static_cast<uint16_t>(shorthand_id))
        , is_shorthand_(true)
    {
    }
    NonCustomPropertyId(LonghandId longhand_id)
        : bit_(static_cast<uint16_t>(longhand_id))
        , is_shorthand_(false)
    {
    }

  public:
    uint32_t bit() const
    {
      return bit_;
    }

    bool operator==(const NonCustomPropertyId &other) const
    {
      return bit_ == other.bit_;
    }
    bool operator!=(const NonCustomPropertyId &other) const
    {
      return !(*this == other);
    }
    operator std::string() const
    {
      if (is_shorthand_)
        return to_string(static_cast<ShorthandId>(bit_));
      else
        return to_string(static_cast<LonghandId>(bit_));
    }
    friend std::ostream &operator<<(std::ostream &os, const NonCustomPropertyId &id)
    {
      os << "NonCustomPropertyId(" << std::string(id) << ")";
      return os;
    }

  private:
    uint16_t bit_;
    bool is_shorthand_ = false;
  };

  class CustomPropertyId
  {
  public:
    CustomPropertyId(const std::string &css)
        : css_(css)
        , custom_bit_(ComputeUniqueHash(css))
    {
    }

  public:
    uint32_t bit() const
    {
      return custom_bit_;
    }

    bool operator==(const CustomPropertyId &other) const
    {
      return custom_bit_ == other.custom_bit_ && css_ == other.css_;
    }
    bool operator!=(const CustomPropertyId &other) const
    {
      return !(*this == other);
    }
    operator std::string() const
    {
      return css_;
    }
    friend std::ostream &operator<<(std::ostream &os, const CustomPropertyId &id)
    {
      os << "CustomPropertyId(" << std::string(id) << ")";
      return os;
    }

  private:
    static uint32_t ComputeUniqueHash(const std::string &str)
    {
      const uint32_t prime = 0x01000193; // 16777619
      uint32_t hash = 0x811C9DC5;        // 2166136261

      // compute hash based on the ASCII values of the characters
      for (char c : str)
      {
        hash ^= static_cast<uint8_t>(c);
        hash *= prime;
      }

      // mixing the characters in a more complex way
      for (size_t i = 0; i < str.size(); ++i)
        hash += (str[i] << (i % 24)) ^ (str[str.size() - 1 - i] >> (i % 8));

      return hash;
    }

  private:
    uint32_t custom_bit_;
    std::string css_;
  };

  class PropertyId
  {
  private:
    enum Tag : uint8_t
    {
      kNonCustom,
      kCustom,
    };
    using IdVariant = std::variant<NonCustomPropertyId, CustomPropertyId>;

  public:
    static PropertyId NonCustom(const NonCustomPropertyId &non_custom_property_id)
    {
      return PropertyId(non_custom_property_id);
    };
    static PropertyId Custom(const CustomPropertyId &custom_property_id)
    {
      return PropertyId(custom_property_id);
    };

  private:
    PropertyId(NonCustomPropertyId non_custom_property_id)
        : tag_(kNonCustom)
        , id_(non_custom_property_id)
    {
    }
    PropertyId(CustomPropertyId custom_property_id)
        : tag_(kCustom)
        , id_(custom_property_id)
    {
    }

  private:
    Tag tag_;
    IdVariant id_;
  };
}
