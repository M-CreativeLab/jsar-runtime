#pragma once

#include <string>
#include <optional>
#include "./length.hpp"

namespace client_cssom::types
{
  enum class LineWidthKeyword
  {
    kThin,
    kMedium,
    kThick,
  };


  enum class BorderStyleKeyword
  {
    kNone,
    kHidden,
    kDotted,
    kDashed,
    kSolid,
    kDouble,
    kGroove,
    kRidge,
    kInset,
    kOutset,
  };

  template <typename T>
  inline std::optional<T> parseKeyword(std::string input)
  {
    return T();
  }

  template <>
  inline std::optional<LineWidthKeyword> parseKeyword(std::string input)
  {
    if (input == "thin")
      return LineWidthKeyword::kThin;
    if (input == "medium")
      return LineWidthKeyword::kMedium;
    if (input == "thick")
      return LineWidthKeyword::kThick;
    return std::nullopt;
  }

  template <>
  inline std::optional<BorderStyleKeyword> parseKeyword(std::string input)
  {
    if (input == "none")
      return BorderStyleKeyword::kNone;
    if (input == "hidden")
      return BorderStyleKeyword::kHidden;
    if (input == "dotted")
      return BorderStyleKeyword::kDotted;
    if (input == "dashed")
      return BorderStyleKeyword::kDashed;
    if (input == "solid")
      return BorderStyleKeyword::kSolid;
    if (input == "double")
      return BorderStyleKeyword::kDouble;
    if (input == "groove")
      return BorderStyleKeyword::kGroove;
    if (input == "ridge")
      return BorderStyleKeyword::kRidge;
    if (input == "inset")
      return BorderStyleKeyword::kInset;
    if (input == "outset")
      return BorderStyleKeyword::kOutset;
    return std::nullopt;
  }
}
