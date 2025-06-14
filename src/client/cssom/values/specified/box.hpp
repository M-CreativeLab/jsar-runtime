#pragma once

#include <cstdint>
#include <crates/bindings.hpp>
#include <client/cssom/style_traits.hpp>

namespace client_cssom::values::specified
{
  class Display;
  class DisplayOutside
  {
    friend class Display;

  private:
    enum Tag : uint8_t
    {
      kNone = 0,
      kInline,
      kBlock,
      kTableCaption,
      kInternalTable,
    };

  public:
    static DisplayOutside None()
    {
      return DisplayOutside(kNone);
    }
    static DisplayOutside Inline()
    {
      return DisplayOutside(kInline);
    }
    static DisplayOutside Block()
    {
      return DisplayOutside(kBlock);
    }
    static DisplayOutside TableCaption()
    {
      return DisplayOutside(kTableCaption);
    }
    static DisplayOutside InternalTable()
    {
      return DisplayOutside(kInternalTable);
    }
    static DisplayOutside InternalTableCaption()
    {
      return DisplayOutside(kTableCaption);
    }

  public:
    DisplayOutside()
        : tag_(kBlock)
    {
    }

  private:
    DisplayOutside(Tag tag)
        : tag_(tag)
    {
    }

  public:
    inline bool isNone() const
    {
      return tag_ == kNone;
    }
    inline bool isInline() const
    {
      return tag_ == kInline;
    }
    inline bool isBlock() const
    {
      return tag_ == kBlock;
    }

  private:
    Tag tag_;
  };

  class DisplayInside
  {
    friend class Display;

  private:
    enum Tag : uint8_t
    {
      kNone = 0,
      kContents,
      kFlow,
      kFlowRoot,
      kFlex,
      kGrid,
      kTable,
      kRuby,
    };

  public:
    static DisplayInside None()
    {
      return DisplayInside(kNone);
    }
    static DisplayInside Contents()
    {
      return DisplayInside(kContents);
    }
    static DisplayInside Flow()
    {
      return DisplayInside(kFlow);
    }
    static DisplayInside FlowRoot()
    {
      return DisplayInside(kFlowRoot);
    }
    static DisplayInside Flex()
    {
      return DisplayInside(kFlex);
    }
    static DisplayInside Grid()
    {
      return DisplayInside(kGrid);
    }
    static DisplayInside Table()
    {
      return DisplayInside(kTable);
    }
    static DisplayInside Ruby()
    {
      return DisplayInside(kRuby);
    }

  public:
    DisplayInside()
        : tag_(kFlow)
    {
    }

  private:
    DisplayInside(Tag tag)
        : tag_(tag)
    {
    }

  public:
    inline bool isNone() const
    {
      return tag_ == kNone;
    }
    inline bool isFlex() const
    {
      return tag_ == kFlex;
    }
    inline bool isGrid() const
    {
      return tag_ == kGrid;
    }
    inline bool isFlexOrGrid() const
    {
      return isFlex() || isGrid();
    }

  private:
    Tag tag_;
  };

  class Display : public Parse,
                  public ToCss,
                  public ToComputedValue<Display>,
                  public ToLayoutValue<crates::layout2::styles::Display>
  {
    friend class Parse;

  public:
    // FIXME(yorkie): not used
    static constexpr uint16_t LIST_ITEM_MASK = 0b1000000000000000;
    static constexpr uint16_t OUTSIDE_MASK = 0b0111111100000000;
    static constexpr uint16_t INSIDE_MASK = 0b0000000011111111;
    static constexpr uint16_t OUTSIDE_SHIFT = 8;

    static Display None()
    {
      return Display(DisplayOutside::kNone, DisplayInside::kNone);
    }
    static Display Contents()
    {
      return Display(DisplayOutside::kNone, DisplayInside::kContents);
    }
    static Display Inline()
    {
      return Display(DisplayOutside::kInline, DisplayInside::kFlow);
    }
    static Display InlineBlock()
    {
      return Display(DisplayOutside::kInline, DisplayInside::kFlowRoot);
    }
    static Display Block()
    {
      return Display(DisplayOutside::kBlock, DisplayInside::kFlow);
    }
    static Display FlowRoot()
    {
      return Display(DisplayOutside::kBlock, DisplayInside::kFlowRoot);
    }
    static Display Flex()
    {
      return Display(DisplayOutside::kBlock, DisplayInside::kFlex);
    }
    static Display InlineFlex()
    {
      return Display(DisplayOutside::kInline, DisplayInside::kFlex);
    }
    static Display Grid()
    {
      return Display(DisplayOutside::kBlock, DisplayInside::kGrid);
    }
    static Display InlineGrid()
    {
      return Display(DisplayOutside::kInline, DisplayInside::kGrid);
    }
    static Display Table()
    {
      return Display(DisplayOutside::kBlock, DisplayInside::kTable);
    }
    static Display InlineTable()
    {
      return Display(DisplayOutside::kInline, DisplayInside::kTable);
    }

  public:
    Display()
        : Display(DisplayOutside::kNone, DisplayInside::kNone)
    {
    }

  private:
    Display(uint8_t outside_bits, uint8_t inside_bits)
        : bits_(static_cast<uint16_t>(outside_bits) << OUTSIDE_SHIFT | static_cast<uint16_t>(inside_bits))
    {
    }

  private:
    bool parse(const std::string &input) override
    {
      if (input == "none")
        bits_ = 0;
      else if (input == "block")
        bits_ = DisplayOutside::kBlock << OUTSIDE_SHIFT | DisplayInside::kFlow;
      else if (input == "inline")
        bits_ = DisplayOutside::kInline << OUTSIDE_SHIFT | DisplayInside::kFlow;
      else if (input == "inline-block")
        bits_ = DisplayOutside::kInline << OUTSIDE_SHIFT | DisplayInside::kFlowRoot;
      else if (input == "flex")
        bits_ = DisplayOutside::kBlock << OUTSIDE_SHIFT | DisplayInside::kFlex;
      else if (input == "inline-flex")
        bits_ = DisplayOutside::kInline << OUTSIDE_SHIFT | DisplayInside::kFlex;
      else if (input == "grid")
        bits_ = DisplayOutside::kBlock << OUTSIDE_SHIFT | DisplayInside::kGrid;
      else if (input == "inline-grid")
        bits_ = DisplayOutside::kInline << OUTSIDE_SHIFT | DisplayInside::kGrid;
      return true;
    }

  public:
    std::string toCss() const override
    {
      return "display";
    }
    Display toComputedValue(computed::Context &) const override
    {
      return *this;
    }
    crates::layout2::styles::Display toLayoutValue() const override
    {
      if (isNone())
        return crates::layout2::styles::Display::None();
      else if (isFlex())
        return crates::layout2::styles::Display::Flex();
      else if (isGrid())
        return crates::layout2::styles::Display::Grid();
      else
        return crates::layout2::styles::Display::Block();
    }

    inline DisplayOutside outside() const
    {
      return DisplayOutside(static_cast<DisplayOutside::Tag>((bits_ & OUTSIDE_MASK) >> OUTSIDE_SHIFT));
    }
    inline DisplayInside inside() const
    {
      return DisplayInside(static_cast<DisplayInside::Tag>(bits_ & INSIDE_MASK));
    }

    inline bool isItemContainer() const
    {
      return inside().isFlexOrGrid();
    }
    inline bool isNone() const
    {
      return outside().isNone() && inside().isNone();
    }
    inline bool isFlex() const
    {
      return outside().isBlock() && inside().isFlex();
    }
    inline bool isGrid() const
    {
      return outside().isBlock() && inside().isGrid();
    }

  private:
    uint16_t bits_ = 0;
  };

  class BoxSizing : public Parse,
                    public ToCss,
                    public ToComputedValue<BoxSizing>,
                    public ToLayoutValue<crates::layout2::styles::BoxSizing>
  {
    friend class Parse;

  private:
    enum Tag : uint8_t
    {
      kContentBox = 0,
      kBorderBox,
    };

  public:
    static BoxSizing ContentBox()
    {
      return BoxSizing(kContentBox);
    }
    static BoxSizing BorderBox()
    {
      return BoxSizing(kBorderBox);
    }

  public:
    BoxSizing()
        : BoxSizing(kContentBox)
    {
    }

  private:
    BoxSizing(Tag tag)
        : tag_(tag)
    {
    }

  private:
    bool parse(const std::string &input) override
    {
      if (input == "content-box")
        tag_ = kContentBox;
      else if (input == "border-box")
        tag_ = kBorderBox;
      return true;
    }

  public:
    std::string toCss() const override
    {
      if (tag_ == kBorderBox)
        return "border-box";
      else
        return "content-box";
    }
    BoxSizing toComputedValue(computed::Context &) const override
    {
      return *this;
    }
    crates::layout2::styles::BoxSizing toLayoutValue() const override
    {
      if (tag_ == kContentBox)
        return crates::layout2::styles::BoxSizing::ContentBox();
      else
        return crates::layout2::styles::BoxSizing::BorderBox();
    }

  private:
    Tag tag_;
  };

  class Overflow : public Parse,
                   public ToCss,
                   public ToComputedValue<Overflow>,
                   public ToLayoutValue<crates::layout2::styles::Overflow>
  {
    friend class Parse;

  private:
    enum Tag : uint8_t
    {
      kVisible = 0,
      kHidden,
      kScroll,
      kAuto,
      kClip,
    };

  public:
    static Overflow Visible()
    {
      return Overflow(kVisible);
    }
    static Overflow Hidden()
    {
      return Overflow(kHidden);
    }
    static Overflow Scroll()
    {
      return Overflow(kScroll);
    }
    static Overflow Auto()
    {
      return Overflow(kAuto);
    }
    static Overflow Clip()
    {
      return Overflow(kClip);
    }

  public:
    Overflow()
        : tag_(kVisible)
    {
    }

  private:
    Overflow(Tag tag)
        : tag_(tag)
    {
    }

    bool parse(const std::string &input) override
    {
      if (input == "visible")
        tag_ = kVisible;
      else if (input == "hidden")
        tag_ = kHidden;
      else if (input == "scroll")
        tag_ = kScroll;
      else if (input == "auto")
        tag_ = kAuto;
      else if (input == "clip")
        tag_ = kClip;
      return true;
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kVisible:
        return "visible";
      case kHidden:
        return "hidden";
      case kScroll:
        return "scroll";
      case kAuto:
        return "auto";
      case kClip:
        return "clip";
      }
      return "";
    }
    Overflow toComputedValue(computed::Context &) const override
    {
      return *this;
    }
    crates::layout2::styles::Overflow toLayoutValue() const override
    {
      switch (tag_)
      {
      case kVisible:
        return crates::layout2::styles::Overflow::Visible();
      case kHidden:
        return crates::layout2::styles::Overflow::Hidden();
      case kScroll:
        return crates::layout2::styles::Overflow::Scroll();
      case kClip:
        return crates::layout2::styles::Overflow::Clip();
      case kAuto:
      default:
        // TODO(yorkie): support the `auto` value.
        return crates::layout2::styles::Overflow::Visible();
      }
    }

    inline bool isVisible() const
    {
      return tag_ == kVisible;
    }
    inline bool isHidden() const
    {
      return tag_ == kHidden;
    }
    inline bool isScroll() const
    {
      return tag_ == kScroll;
    }
    inline bool isAuto() const
    {
      return tag_ == kAuto;
    }
    inline bool isClip() const
    {
      return tag_ == kClip;
    }

    // Compbined checks
    inline bool isAutoOrScroll() const
    {
      return isAuto() || isScroll();
    }
    // Returns if the overflow is scrollable, namely `auto`, `scroll`, or `hidden`.
    // NOTE(yorkie): `hidden` is considered scrollable because it can be scrolled by JavaScript.
    inline bool isScrollable() const
    {
      return !isVisible() && !isClip();
    }

  private:
    Tag tag_;
  };
}
