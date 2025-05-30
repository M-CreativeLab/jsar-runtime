#pragma once

#include <cstdint>
#include <concepts>

namespace client_cssom::values::generics
{
  template <typename T>
  class GenericFlexDirection
  {
  protected:
    enum Tag
    {
      kRow = 0,
      kRowReverse,
      kColumn,
      kColumnReverse,
    };

  public:
    static T Row() { return T(Tag::kRow); }
    static T RowReverse() { return T(Tag::kRowReverse); }
    static T Column() { return T(Tag::kColumn); }
    static T ColumnReverse() { return T(Tag::kColumnReverse); }

  public:
    GenericFlexDirection() : tag_(kRow) {}

  private:
    GenericFlexDirection(Tag tag)
        : tag_(tag)
    {
    }

  protected:
    Tag tag_;
  };

  template <typename T>
  class GenericFlexWrap
  {
  protected:
    enum Tag
    {
      kNoWrap = 0,
      kWrap,
      kWrapReverse,
    };

  public:
    static T NoWrap() { return T(Tag::kNoWrap); }
    static T Wrap() { return T(Tag::kWrap); }
    static T WrapReverse() { return T(Tag::kWrapReverse); }

  public:
    GenericFlexWrap() : tag_(kNoWrap) {}

  private:
    GenericFlexWrap(Tag tag)
        : tag_(tag)
    {
    }

  protected:
    Tag tag_;
  };
}
