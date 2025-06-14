#pragma once

#include <string>
#include <regex>

using namespace std;

namespace font
{
  static string FONT_SHORTHAND_REGEX =
    "(italic|oblique|normal|)\\s*"              // style
    "(small-caps|normal|)\\s*"                  // variant
    "(bold|bolder|lighter|[1-9]00|normal|)\\s*" // weight
    "([\\d\\.]+)"                               // size
    "(px|pt|pc|in|cm|mm|%|em|ex|ch|rem|q)"      // unit
    "(.+)";                                     // family

  inline string ltrim(string s)
  {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch)
                               { return !isspace(ch); }));
    return s;
  }

  class FontShorthandParser
  {
  public:
    FontShorthandParser(const string &input, double defaultHeight = 16.0)
    {
      parseInto(input, defaultHeight);
    }

  private:
    void parseInto(const string &input, double defaultHeight)
    {
      regex rFontShorthand(FONT_SHORTHAND_REGEX);
      smatch matches;

      if (!regex_search(input, matches, rFontShorthand))
        return;

      string styleStr = matches[1];
      variant = matches[2];
      weight = matches[3];
      auto sizeNumber = stod(matches[4]);
      auto sizeUnit = matches[5];
      family = ltrim(matches[6]);

      if (styleStr == "normal")
        style = SkFontStyle::Normal();
      else if (styleStr == "italic")
        style = SkFontStyle::Italic();
      else
        style = SkFontStyle::Normal();

      if (sizeUnit == "em" || sizeUnit == "rem")
        sizeInPx = sizeNumber * defaultHeight;
      else if (sizeUnit == "pt")
        sizeInPx = sizeNumber * 4 / 3;
      else if (sizeUnit == "px" || sizeUnit == "pc")
        sizeInPx = sizeNumber;
      else if (sizeUnit == "in")
        sizeInPx = sizeNumber * 96;
      else if (sizeUnit == "cm")
        sizeInPx = sizeNumber * 96 / 2.54;
      else if (sizeUnit == "mm")
        sizeInPx = sizeNumber * 96 / 25.4;
      else if (sizeUnit == "%")
        sizeInPx = sizeNumber * (defaultHeight / 75);
      else if (sizeUnit == "q")
        sizeInPx = sizeNumber * (96 / 25.4 / 4);
      success = true;
    }

  public:
    string family;
    SkFontStyle style;
    string variant;
    string weight;
    double sizeInPx = 0.0;
    bool success = false;
  };
}
