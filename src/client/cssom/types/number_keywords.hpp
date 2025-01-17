#pragma once

#include <concepts>
#include <string>
#include <optional>
#include <skia/modules/skparagraph/include/TextStyle.h>

#include "./keyword.hpp"
#include "./length.hpp"

namespace client_cssom::types
{
  template <typename K, typename V>
    requires is_keyword<K> &&
             (std::is_integral_v<V> || std::is_floating_point_v<V>)
  class NumberKeyword
  {
  public:
    NumberKeyword() : value_(0), keyword_(std::nullopt)
    {
    }
    NumberKeyword(V value) : value_(value), keyword_(std::nullopt)
    {
    }
    NumberKeyword(K keyword) : keyword_(keyword)
    {
    }

  public:
    inline V value() const { return value_; }
    inline std::optional<K> keyword() const { return keyword_; }
    inline bool hasKeyword() const { return keyword_.has_value(); }

  public:
    operator V() const { return value_; }

  protected:
    V value_;
    std::optional<K> keyword_;
  };

  class FontWeight : public NumberKeyword<FontWeightKeyword, int>
  {
    using NumberKeyword::NumberKeyword;

  public:
    FontWeight(FontWeightKeyword keyword)
        : NumberKeyword(keyword)
    {
      if (keyword_ == FontWeightKeyword::kNormal)
        value_ = 400;
      else if (keyword_ == FontWeightKeyword::kBold)
        value_ = 700;
      else if (keyword_ == FontWeightKeyword::kBolder)
        value_ = 900;
      else if (keyword_ == FontWeightKeyword::kLighter)
        value_ = 100;
    }
    FontWeight(const std::string &input)
    {
      auto keyword = parseKeyword<FontWeightKeyword>(input);
      if (keyword.has_value())
        FontWeight::operator=(FontWeight(keyword.value()));
      else
        NumberKeyword::operator=(NumberKeyword<FontWeightKeyword, int>(std::stoi(input)));
    }
  };
}
