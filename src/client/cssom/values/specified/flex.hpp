#pragma once

#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/generics/flex.hpp>
#include <client/cssom/values/computed/flex.hpp>

namespace client_cssom::values::specified
{
  class FlexDirection : public generics::GenericFlexDirection<FlexDirection>,
                        public Parse,
                        public ToComputedValue<computed::FlexDirection>
  {
    friend class Parse;

  private:
    bool parse(const std::string &input) override
    {
      if (input == "row")
        tag_ = kRow;
      else if (input == "row-reverse")
        tag_ = kRowReverse;
      else if (input == "column")
        tag_ = kColumn;
      else if (input == "column-reverse")
        tag_ = kColumnReverse;
      else
        tag_ = kRow;
      return true;
    }

  public:
    computed::FlexDirection toComputedValue(computed::Context &context) const override
    {
      switch (tag_)
      {
      case kRow:
        return computed::FlexDirection::Row();
      case kRowReverse:
        return computed::FlexDirection::RowReverse();
      case kColumn:
        return computed::FlexDirection::Column();
      case kColumnReverse:
        return computed::FlexDirection::ColumnReverse();
      default:
        return computed::FlexDirection::Row();
      }
    }
  };
}
