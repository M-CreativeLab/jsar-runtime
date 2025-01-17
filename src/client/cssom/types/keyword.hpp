#pragma once

#include <concepts>
#include <string>
#include <optional>
#include "./length.hpp"

namespace client_cssom::types
{
#define LINE_WIDTH_KEYWORDS_MAP(XX) \
  XX("thin", kThin)                 \
  XX("medium", kMedium)             \
  XX("thick", kThick)

#define BORDER_STYLE_KEYWORDS_MAP(XX) \
  XX("none", kNone)                   \
  XX("hidden", kHidden)               \
  XX("dotted", kDotted)               \
  XX("dashed", kDashed)               \
  XX("solid", kSolid)                 \
  XX("double", kDouble)               \
  XX("groove", kGroove)               \
  XX("ridge", kRidge)                 \
  XX("inset", kInset)                 \
  XX("outset", kOutset)

#define DIR_KEYWORDS_MAP(XX) \
  XX("ltr", kLtr)            \
  XX("rtl", kRtl)

#define FONT_STYLE_KEYWORDS_MAP(XX) \
  XX("normal", kNormal)             \
  XX("italic", kItalic)             \
  XX("oblique", kOblique)

#define FONT_WEIGHT_KEYWORDS_MAP(XX) \
  XX("normal", kNormal)              \
  XX("bold", kBold)                  \
  XX("bolder", kBolder)              \
  XX("lighter", kLighter)

#define TEXT_ALIGN_KEYWORDS_MAP(XX) \
  XX("left", kLeft)                 \
  XX("right", kRight)               \
  XX("center", kCenter)             \
  XX("justify", kJustify)           \
  XX("start", kStart)               \
  XX("end", kEnd)

  enum class LineWidthKeyword
  {
#define XX(_, KEY) KEY,
    LINE_WIDTH_KEYWORDS_MAP(XX)
#undef XX
  };

  enum class BorderStyleKeyword
  {
#define XX(_, KEY) KEY,
    BORDER_STYLE_KEYWORDS_MAP(XX)
#undef XX
  };

  enum class DirectionKeyword
  {
#define XX(_, KEY) KEY,
    DIR_KEYWORDS_MAP(XX)
#undef XX
  };

  enum class FontStyleKeyword
  {
#define XX(_, KEY) KEY,
    FONT_STYLE_KEYWORDS_MAP(XX)
#undef XX
  };

  enum class FontWeightKeyword
  {
#define XX(_, KEY) KEY,
    FONT_WEIGHT_KEYWORDS_MAP(XX)
#undef XX
  };

  enum class TextAlignKeyword
  {
#define XX(_, KEY) KEY,
    TEXT_ALIGN_KEYWORDS_MAP(XX)
#undef XX
  };

#define KEYWORD_MAPS_CASE(XX)                               \
  if constexpr (std::is_same_v<T, LineWidthKeyword>)        \
  {                                                         \
    LINE_WIDTH_KEYWORDS_MAP(XX)                             \
  }                                                         \
  else if constexpr (std::is_same_v<T, BorderStyleKeyword>) \
  {                                                         \
    BORDER_STYLE_KEYWORDS_MAP(XX)                           \
  }                                                         \
  else if constexpr (std::is_same_v<T, DirectionKeyword>)   \
  {                                                         \
    DIR_KEYWORDS_MAP(XX)                                    \
  }                                                         \
  else if constexpr (std::is_same_v<T, FontStyleKeyword>)   \
  {                                                         \
    FONT_STYLE_KEYWORDS_MAP(XX)                             \
  }                                                         \
  else if constexpr (std::is_same_v<T, FontWeightKeyword>)  \
  {                                                         \
    FONT_WEIGHT_KEYWORDS_MAP(XX)                            \
  }                                                         \
  else if constexpr (std::is_same_v<T, TextAlignKeyword>)   \
  {                                                         \
    TEXT_ALIGN_KEYWORDS_MAP(XX)                             \
  }

  template <typename T>
  concept is_keyword = std::is_same_v<T, LineWidthKeyword> ||
                       std::is_same_v<T, BorderStyleKeyword> ||
                       std::is_same_v<T, DirectionKeyword> ||
                       std::is_same_v<T, FontStyleKeyword> ||
                       std::is_same_v<T, FontWeightKeyword> ||
                       std::is_same_v<T, TextAlignKeyword>;

  template <typename T>
    requires is_keyword<T>
  inline std::optional<T> parseKeyword(std::string input)
  {
#define XX(NAME, KEY) \
  if (input == NAME)  \
    return T::KEY;

    KEYWORD_MAPS_CASE(XX)
#undef XX

    // By default, return an empty optional.
    return std::nullopt;
  }

  template <typename T>
    requires is_keyword<T>
  inline T parseKeywordOrValue(std::string input, T defaultValue)
  {
    auto keyword = parseKeyword<T>(input);
    return keyword.has_value()
               ? keyword.value()
               : defaultValue;
  }

  template <typename T>
    requires is_keyword<T>
  inline std::string to_string(T keyword)
  {
#define XX(NAME, KEY)    \
  if (keyword == T::KEY) \
    return NAME;

    KEYWORD_MAPS_CASE(XX)
#undef XX

    // By default, return an empty string.
    return "";
  }

  template <typename T>
    requires is_keyword<T>
  std::ostream &operator<<(std::ostream &os, const T &keyword)
  {
    os << to_string(keyword);
    return os;
  }

  // The `KeywordType` class is a base template class for keyword
  template <typename T, typename K, K defaultValue>
  class KeywordType
  {
  public:
    KeywordType(K keyword = defaultValue)
        : keyword_(keyword)
    {
    }
    KeywordType(const std::string &input)
        : keyword_(parseKeywordOrValue<K>(input, defaultValue))
    {
    }

  public:
    friend std::ostream &operator<<(std::ostream &os, const T &instance)
    {
      os << instance.keyword_;
      return os;
    }

  protected:
    K keyword_;
  };
}
