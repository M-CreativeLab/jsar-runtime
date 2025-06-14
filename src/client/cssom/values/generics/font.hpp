#pragma once

#include <variant>
#include <optional>

namespace client_cssom::values::generics
{
  template <typename T, typename AngleType>
  class FontStyle
  {
  protected:
    enum Tag : uint8_t
    {
      kNormal,
      kItalic,
      kOblique,
    };

  public:
    static T Normal()
    {
      return T(kNormal);
    }
    static T Italic()
    {
      return T(kItalic);
    }
    static T Oblique(AngleType angle)
    {
      return T(kOblique, angle);
    }

  protected:
    FontStyle(Tag tag = kNormal, std::optional<AngleType> oblique_angle = std::nullopt)
        : tag_(tag)
        , oblique_angle_(oblique_angle)
    {
    }

  protected:
    Tag tag_;
    std::optional<AngleType> oblique_angle_;
  };

  template <typename T, typename N, typename L>
  class GenericLineHeight
  {
  private:
    enum Tag : uint8_t
    {
      kNormal,
      kNumber,
      kLength,
    };
    using ValueVariant = std::variant<std::monostate, N, L>;

  public:
    static T Normal()
    {
      return T(kNormal);
    }
    static T Number(N number)
    {
      return T(kNumber, number);
    }
    static T Length(L length)
    {
      return T(kLength, length);
    }

  public:
    inline bool isNormal() const
    {
      return tag_ == kNormal;
    }
    inline bool isNumber() const
    {
      return tag_ == kNumber;
    }
    inline bool isLength() const
    {
      return tag_ == kLength;
    }

    N getNumber()
    {
      if (isNumber())
        return std::get<N>(value_);
      throw std::bad_variant_access();
    }
    const N &getNumber() const
    {
      if (isNumber())
        return std::get<N>(value_);
      throw std::bad_variant_access();
    }
    L getLength()
    {
      if (isLength())
        return std::get<L>(value_);
      throw std::bad_variant_access();
    }
    const L &getLength() const
    {
      if (isLength())
        return std::get<L>(value_);
      throw std::bad_variant_access();
    }

  protected:
    GenericLineHeight()
        : tag_(Tag::kNormal)
        , value_(std::monostate{})
    {
    }
    GenericLineHeight(Tag tag, ValueVariant value = std::monostate{})
        : tag_(tag)
        , value_(value)
    {
    }

    void setNormal()
    {
      tag_ = Tag::kNormal;
      value_ = std::monostate{};
    }
    void setNumber(N number)
    {
      tag_ = Tag::kNumber;
      value_ = number;
    }
    void setLength(L length)
    {
      tag_ = Tag::kLength;
      value_ = length;
    }

  private:
    Tag tag_;
    ValueVariant value_;
  };
}
