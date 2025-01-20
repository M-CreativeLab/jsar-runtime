#pragma once

#include <cctype>
#include <string>
#include <iostream>
#include <assert.h>
#include "./length.hpp"

namespace client_cssom::types
{
  class NumberLengthPercentage : public LengthPercentage
  {
  private:
    enum class _ValueType
    {
      kNumber,
      kLength,
      kPercentage,
    };

  public:
    static NumberLengthPercentage Number(float value)
    {
      return NumberLengthPercentage(value);
    }
    static NumberLengthPercentage Length(float value, Unit unit)
    {
      return NumberLengthPercentage(value, unit);
    }
    static NumberLengthPercentage Percentage(float value)
    {
      return NumberLengthPercentage(value);
    }

  public:
    NumberLengthPercentage(float value = 0.0f)
        : LengthPercentage(),
          number_(value),
          type_(_ValueType::kNumber)
    {
    }
    NumberLengthPercentage(float value, Unit unit)
        : LengthPercentage(value, unit),
          type_(_ValueType::kLength)
    {
    }
    NumberLengthPercentage(const std::string &input)
    {
      try
      {
        size_t endsAt = 0;
        number_ = std::stof(input, &endsAt);
        if (endsAt != input.size())
          throw std::invalid_argument("Invalid number");
        type_ = _ValueType::kNumber;
      }
      catch (const std::invalid_argument &e)
      {
        LengthPercentage::operator=(LengthPercentage(input));
        type_ = percentage_.has_value()
                    ? _ValueType::kPercentage
                    : _ValueType::kLength;
      }
    }

  public:
    friend std::ostream &operator<<(std::ostream &os,
                                    const NumberLengthPercentage &value)
    {
      if (value.type_ == _ValueType::kNumber)
        os << value.number_;
      else
        os << static_cast<const LengthPercentage &>(value);
      return os;
    }

  public:
    inline bool isNumber() const { return type_ == _ValueType::kNumber; }
    inline bool isLength() const { return type_ == _ValueType::kLength; }
    inline bool isPercentage() const { return type_ == _ValueType::kPercentage; }
    inline float number() const
    {
      assert(type_ == _ValueType::kNumber);
      return number_;
    }

  private:
    float number_;
    _ValueType type_;
  };
}
