#pragma once

#include <optional>
#include <variant>
#include <crates/bindings.hpp>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/generics/common.hpp>
#include <client/cssom/values/computed/common.hpp>
#include <client/cssom/values/computed/percentage.hpp>

namespace client_cssom::values::computed
{
  class LengthPercentage : public ToLayoutValue<crates::layout2::styles::LengthPercentage>
  {
  private:
    enum Tag : uint8_t
    {
      kLength,
      kPercentage,
      kCalc,
    };

    struct LengthVariant
    {
      Tag tag;
      CSSPixelLength length;

      static LengthVariant From(const CSSPixelLength &length)
      {
        return LengthVariant{Tag::kLength, length};
      }
    };

    struct PercentageVariant
    {
      Tag tag;
      Percentage percentage;

      static PercentageVariant From(const Percentage &percentage)
      {
        return PercentageVariant{Tag::kPercentage, percentage};
      }
    };

    struct CalcVariant
    {
      Tag tag;
      void *calc_ptr;
    };

    using Variant = std::variant<LengthVariant, PercentageVariant, CalcVariant>;

  public:
    static LengthPercentage Length(const CSSPixelLength &length)
    {
      return LengthPercentage(LengthVariant::From(length));
    }
    static LengthPercentage Percentage(const Percentage &percentage)
    {
      return LengthPercentage(PercentageVariant::From(percentage));
    }

  public:
    LengthPercentage(float value = 0.0f)
        : variant_(LengthVariant::From(CSSPixelLength(value)))
    {
    }
    LengthPercentage(const LengthVariant length)
        : variant_(length)
    {
    }
    LengthPercentage(const PercentageVariant percentage)
        : variant_(percentage)
    {
    }

  public:
    bool operator==(const LengthPercentage &other) const
    {
      if (isLength() && other.isLength())
        return getLength() == other.getLength();
      else if (isPercentage() && other.isPercentage())
        return getPercentage() == other.getPercentage();
      else
        return false;
    }
    bool operator==(float other) const
    {
      if (isLength())
        return getLength() == other;
      else if (isPercentage())
        return getPercentage() == other;
      else
        return false;
    }
    bool operator<(const LengthPercentage &other) const
    {
      if (isLength() && other.isLength())
        return getLength() < other.getLength();
      else if (isPercentage() && other.isPercentage())
        return getPercentage() < other.getPercentage();
      else
        return false;
    }
    bool operator<(float other) const
    {
      if (isLength())
        return getLength() < other;
      else if (isPercentage())
        return getPercentage() < other;
      else
        return false;
    }
    bool operator>(const LengthPercentage &other) const
    {
      if (isLength() && other.isLength())
        return getLength() > other.getLength();
      else if (isPercentage() && other.isPercentage())
        return getPercentage() > other.getPercentage();
      else
        return false;
    }
    bool operator>(float other) const
    {
      if (isLength())
        return getLength() > other;
      else if (isPercentage())
        return getPercentage() > other;
      else
        return false;
    }

  public:
    inline bool isLength() const
    {
      return std::holds_alternative<LengthVariant>(variant_);
    }
    inline bool isPercentage() const
    {
      return std::holds_alternative<PercentageVariant>(variant_);
    }
    inline bool isCalc() const
    {
      return std::holds_alternative<CalcVariant>(variant_);
    }

    inline CSSPixelLength getLength() const
    {
      return isLength()
               ? std::get<LengthVariant>(variant_).length
               : CSSPixelLength(0.0f);
    }
    inline computed::Percentage getPercentage() const
    {
      return isPercentage()
               ? std::get<PercentageVariant>(variant_).percentage
               : computed::Percentage(0.0f);
    }

    // Converts this LengthPercentage to a Percentage if it is a percentage.
    inline std::optional<computed::Percentage> toPercentage() const
    {
      if (isPercentage())
        return std::get<PercentageVariant>(variant_).percentage;
      return std::nullopt;
    }

    crates::layout2::styles::LengthPercentage toLayoutValue() const
    {
      if (isLength())
        return crates::layout2::styles::LengthPercentage::Length(getLength().px());
      else if (isPercentage())
        return crates::layout2::styles::LengthPercentage::Percentage(getPercentage().value());
      else
        return crates::layout2::styles::LengthPercentage::Length(0.0f);
    }

  private:
    Variant variant_;
  };

  using NonNegativeLengthPercentage = generics::NonNegative<LengthPercentage>;
}
