#pragma once

#include <crates/bindings.hpp>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/generics/flex.hpp>

namespace client_cssom::values::computed
{
  class FlexDirection : public generics::GenericFlexDirection<FlexDirection>,
                        public ToLayoutValue<crates::layout2::styles::FlexDirection>
  {
    using generics::GenericFlexDirection<FlexDirection>::GenericFlexDirection;

  public:
    crates::layout2::styles::FlexDirection toLayoutValue() const override
    {
      switch (tag_)
      {
      case kRow:
        return crates::layout2::styles::FlexDirection::Row();
      case kRowReverse:
        return crates::layout2::styles::FlexDirection::RowReverse();
      case kColumn:
        return crates::layout2::styles::FlexDirection::Column();
      case kColumnReverse:
        return crates::layout2::styles::FlexDirection::ColumnReverse();
      default:
        return crates::layout2::styles::FlexDirection::Row();
      }
    }
  };

  class FlexWrap : public generics::GenericFlexWrap<FlexWrap>,
                   public ToLayoutValue<crates::layout2::styles::FlexWrap>
  {
    using generics::GenericFlexWrap<FlexWrap>::GenericFlexWrap;

  public:
    crates::layout2::styles::FlexWrap toLayoutValue() const override
    {
      switch (tag_)
      {
      case kNoWrap:
        return crates::layout2::styles::FlexWrap::NoWrap();
      case kWrap:
        return crates::layout2::styles::FlexWrap::Wrap();
      case kWrapReverse:
        return crates::layout2::styles::FlexWrap::WrapReverse();
      default:
        return crates::layout2::styles::FlexWrap::NoWrap();
      }
    }
  };
}
