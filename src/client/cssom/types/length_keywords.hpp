#pragma once

#include <string>
#include <optional>
#include "./keyword.hpp"
#include "./length.hpp"

namespace client_cssom::types
{
  template <typename KT>
  class LengthKeyword : public Length
  {
  public:
    using Length::Length;

  public:
    LengthKeyword(KT keyword) : Length(), keyword_(keyword)
    {
    }

  public:
    inline KT keyword() const { return keyword_; }

  protected:
    KT keyword_;
  };

  // See https://developer.mozilla.org/en-US/docs/Web/CSS/border-width#line-width
  class LineWidth : public LengthKeyword<LineWidthKeyword>
  {
  public:
    using LengthKeyword::LengthKeyword;

  public:
    LineWidth(LineWidthKeyword keyword) : LengthKeyword(keyword)
    {
      switch (keyword)
      {
      case LineWidthKeyword::kThin:
        value() = 1.0f;
        unit() = Unit::kPx;
        break;
      case LineWidthKeyword::kMedium:
        value() = 3.0f;
        unit() = Unit::kPx;
        break;
      case LineWidthKeyword::kThick:
        value() = 5.0f;
        unit() = Unit::kPx;
        break;
      }
    }
    LineWidth(const std::string &input)
    {
      auto keyword = parseKeyword<LineWidthKeyword>(input);
      if (keyword.has_value())
        LineWidth::operator=(LineWidth(keyword.value()));
      else
        Length::operator=(Length(input));
    }
  };
}
