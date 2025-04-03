#pragma once

#include <optional>
#include <string>
#include <ostream>

namespace client_layout
{
  /**
   * The <display-outside> CSS data type specifies the type of outer container box of a box. It is used to determine
   * the type of layout model that will be used for the box. The <display-outside> data type can be one of the
   * following:
   *
   * - block: This value indicates that the box is a block-level box.
   * - inline: This value indicates that the box is an inline-level box.
   * - run-in: This value indicates that the box is a run-in box.
   */
  enum class DisplayOutside
  {
    kBlock,
    kInline,
    kRunIn,
  };

  /**
   * The <display-inside> CSS data type specifies the type of container box of a box. It is used to determine the type
   * of layout model that will be used for the box. The <display-inside> data type can be one of the following:
   *
   * - flow: This value indicates that the box is a block container box, a flex container box, a grid container box,
   *   or a ruby container box, depending on the value of the display-outside property.
   * - flow-root: This value indicates that the box establishes a new block formatting context.
   * - table: This value indicates that the box is a table wrapper box.
   * - flex: This value indicates that the box is a flex container box.
   * - grid: This value indicates that the box is a grid container box.
   */
  enum class DisplayInside
  {
    kFlow,
    kFlowRoot,
    kTable,
    kFlex,
    kGrid,
  };

  enum class DisplayNoneOrContents
  {
    kNone,
    kContents,
  };

  struct DisplayType
  {
    DisplayOutside outside;
    DisplayInside inside;
    // TODO: support <display-listitem> and <display-internal>
    std::optional<DisplayNoneOrContents> noneOrContents = std::nullopt;

    DisplayType(DisplayOutside outside = DisplayOutside::kBlock,
                DisplayInside inside = DisplayInside::kFlow)
        : outside(outside), inside(inside), noneOrContents(std::nullopt)
    {
    }
    DisplayType(DisplayNoneOrContents noneOrContents)
        : noneOrContents(noneOrContents)
    {
    }

    inline bool isNone() const { return noneOrContents == DisplayNoneOrContents::kNone; }
    inline bool isContents() const { return noneOrContents == DisplayNoneOrContents::kContents; }
    inline bool isInline() const { return outside == DisplayOutside::kInline; }
    inline bool isBlock() const { return outside == DisplayOutside::kBlock; }
    inline bool isFlex() const { return inside == DisplayInside::kFlex; }
    inline bool isGrid() const { return inside == DisplayInside::kGrid; }

    friend std::ostream &operator<<(std::ostream &os, const DisplayType &display);

    static DisplayType Make(const std::string &input);
    static DisplayType None() { return DisplayType(DisplayNoneOrContents::kNone); }
    static DisplayType Block(DisplayInside inside = DisplayInside::kFlow)
    {
      return DisplayType(DisplayOutside::kBlock, inside);
    }
    static DisplayType Inline(DisplayInside inside = DisplayInside::kFlow)
    {
      return DisplayType(DisplayOutside::kInline, inside);
    }
    static DisplayType Flex() { return DisplayType(DisplayOutside::kBlock, DisplayInside::kFlex); }
    static DisplayType Grid() { return DisplayType(DisplayOutside::kBlock, DisplayInside::kGrid); }
  };
}
