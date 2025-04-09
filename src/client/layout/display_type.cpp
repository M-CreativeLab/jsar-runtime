#include "./display_type.hpp"

namespace client_layout
{
  using namespace std;

  DisplayType DisplayType::Make(const string &input)
  {
    if (input == "none")
      return DisplayType(DisplayNoneOrContents::kNone);
    if (input == "contents")
      return DisplayType(DisplayNoneOrContents::kContents);
    if (input == "block")
      return DisplayType(DisplayOutside::kBlock, DisplayInside::kFlow);
    if (input == "flow-root")
      return DisplayType(DisplayOutside::kBlock, DisplayInside::kFlowRoot);
    if (input == "inline")
      return DisplayType(DisplayOutside::kInline, DisplayInside::kFlow);
    if (input == "inline-block")
      return DisplayType(DisplayOutside::kInline, DisplayInside::kFlowRoot);
    if (input == "run-in")
      return DisplayType(DisplayOutside::kRunIn, DisplayInside::kFlow);
    if (input == "flex")
      return DisplayType(DisplayOutside::kBlock, DisplayInside::kFlex);
    if (input == "inline-flex")
      return DisplayType(DisplayOutside::kInline, DisplayInside::kFlex);
    if (input == "grid")
      return DisplayType(DisplayOutside::kBlock, DisplayInside::kGrid);
    if (input == "inline-grid")
      return DisplayType(DisplayOutside::kInline, DisplayInside::kGrid);
    if (input == "table")
      return DisplayType(DisplayOutside::kBlock, DisplayInside::kTable);
    if (input == "inline-table")
      return DisplayType(DisplayOutside::kInline, DisplayInside::kTable);
    return DisplayType();
  }

  std::ostream &operator<<(std::ostream &os, const DisplayType &display)
  {
    if (display.noneOrContents.has_value())
    {
      if (display.noneOrContents.value() == DisplayNoneOrContents::kNone)
        os << "Display(none)";
      else
        os << "Display(contents)";
    }
    else
    {
      os << "Display(";
      os << "outside=";
      switch (display.outside)
      {
      case DisplayOutside::kBlock:
        os << "block";
        break;
      case DisplayOutside::kInline:
        os << "inline";
        break;
      case DisplayOutside::kRunIn:
        os << "run-in";
        break;
      default:
        os << "unknown";
        break;
      }

      os << ", inside=";
      switch (display.inside)
      {
      case DisplayInside::kFlow:
        os << "flow";
        break;
      case DisplayInside::kFlowRoot:
        os << "flow-root";
        break;
      case DisplayInside::kTable:
        os << "table";
        break;
      case DisplayInside::kFlex:
        os << "flex";
        break;
      case DisplayInside::kGrid:
        os << "grid";
        break;
      default:
        os << "unknown";
        break;
      }
    }
    return os;
  }
}
