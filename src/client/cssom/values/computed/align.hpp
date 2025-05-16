#pragma once

#include <crates/bindings.hpp>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/specified/align.hpp>

namespace client_cssom::values::computed
{
  class AlignContent : public specified::AlignContent,
                       public ToLayoutValue<crates::layout2::styles::AlignContent>
  {
    using specified::AlignContent::AlignContent;

  public:
    AlignContent(specified::AlignContent other) : specified::AlignContent(other)
    {
    }

    crates::layout2::styles::AlignContent toLayoutValue() const override
    {
      switch (tag())
      {
      case specified::AlignContent::Tag::kStart:
        return crates::layout2::styles::AlignContent::Start();
      case specified::AlignContent::Tag::kEnd:
        return crates::layout2::styles::AlignContent::End();
      case specified::AlignContent::Tag::kCenter:
        return crates::layout2::styles::AlignContent::Center();
      case specified::AlignContent::Tag::kFlexStart:
        return crates::layout2::styles::AlignContent::FlexStart();
      case specified::AlignContent::Tag::kFlexEnd:
        return crates::layout2::styles::AlignContent::FlexEnd();
      case specified::AlignContent::Tag::kSpaceBetween:
        return crates::layout2::styles::AlignContent::SpaceBetween();
      case specified::AlignContent::Tag::kSpaceAround:
        return crates::layout2::styles::AlignContent::SpaceAround();
      case specified::AlignContent::Tag::kSpaceEvenly:
        return crates::layout2::styles::AlignContent::SpaceEvenly();
      case specified::AlignContent::Tag::kStretch:
        return crates::layout2::styles::AlignContent::Stretch();
      default:
        return crates::layout2::styles::AlignContent::Start();
      }
    }
  };

  class JustifyContent : public specified::JustifyContent,
                         public ToLayoutValue<crates::layout2::styles::JustifyContent>
  {
    using specified::JustifyContent::JustifyContent;

  public:
    JustifyContent(specified::JustifyContent other) : specified::JustifyContent(other)
    {
    }

    crates::layout2::styles::JustifyContent toLayoutValue() const override
    {
      switch (tag())
      {
      case specified::JustifyContent::Tag::kStart:
        return crates::layout2::styles::JustifyContent::Start();
      case specified::JustifyContent::Tag::kEnd:
        return crates::layout2::styles::JustifyContent::End();
      case specified::JustifyContent::Tag::kCenter:
        return crates::layout2::styles::JustifyContent::Center();
      case specified::JustifyContent::Tag::kFlexStart:
        return crates::layout2::styles::JustifyContent::FlexStart();
      case specified::JustifyContent::Tag::kFlexEnd:
        return crates::layout2::styles::JustifyContent::FlexEnd();
      case specified::JustifyContent::Tag::kStretch:
        return crates::layout2::styles::JustifyContent::Stretch();
      default:
        return crates::layout2::styles::JustifyContent::Start();
      }
    }
  };

  class AlignItems : public specified::AlignItems,
                     public ToLayoutValue<crates::layout2::styles::AlignItems>
  {
    using specified::AlignItems::AlignItems;

  public:
    AlignItems(specified::AlignItems other) : specified::AlignItems(other)
    {
    }

    crates::layout2::styles::AlignItems toLayoutValue() const override
    {
      switch (tag())
      {
      case specified::AlignItems::Tag::kStart:
        return crates::layout2::styles::AlignItems::Start();
      case specified::AlignItems::Tag::kEnd:
        return crates::layout2::styles::AlignItems::End();
      case specified::AlignItems::Tag::kCenter:
        return crates::layout2::styles::AlignItems::Center();
      case specified::AlignItems::Tag::kFlexStart:
        return crates::layout2::styles::AlignItems::FlexStart();
      case specified::AlignItems::Tag::kFlexEnd:
        return crates::layout2::styles::AlignItems::FlexEnd();
      case specified::AlignItems::Tag::kStretch:
        return crates::layout2::styles::AlignItems::Stretch();
      default:
        return crates::layout2::styles::AlignItems::Start();
      }
    }
  };

  class AlignSelf : public specified::AlignSelf,
                    public ToLayoutValue<crates::layout2::styles::AlignSelf>
  {
    using specified::AlignSelf::AlignSelf;

  public:
    AlignSelf(specified::AlignSelf other) : specified::AlignSelf(other)
    {
    }

    crates::layout2::styles::AlignSelf toLayoutValue() const override
    {
      switch (tag())
      {
      case specified::AlignSelf::Tag::kStart:
        return crates::layout2::styles::AlignSelf::Start();
      case specified::AlignSelf::Tag::kEnd:
        return crates::layout2::styles::AlignSelf::End();
      case specified::AlignSelf::Tag::kCenter:
        return crates::layout2::styles::AlignSelf::Center();
      case specified::AlignSelf::Tag::kFlexStart:
        return crates::layout2::styles::AlignSelf::FlexStart();
      case specified::AlignSelf::Tag::kFlexEnd:
        return crates::layout2::styles::AlignSelf::FlexEnd();
      case specified::AlignSelf::Tag::kStretch:
        return crates::layout2::styles::AlignSelf::Stretch();
      default:
        return crates::layout2::styles::AlignSelf::Start();
      }
    }
  };

  class JustifySelf : public specified::JustifySelf,
                      public ToLayoutValue<crates::layout2::styles::JustifySelf>
  {
    using specified::JustifySelf::JustifySelf;

  public:
    JustifySelf(specified::JustifySelf other) : specified::JustifySelf(other)
    {
    }

    crates::layout2::styles::JustifySelf toLayoutValue() const override
    {
      switch (tag())
      {
      case specified::JustifySelf::Tag::kStart:
        return crates::layout2::styles::JustifySelf::Start();
      case specified::JustifySelf::Tag::kEnd:
        return crates::layout2::styles::JustifySelf::End();
      case specified::JustifySelf::Tag::kCenter:
        return crates::layout2::styles::JustifySelf::Center();
      case specified::JustifySelf::Tag::kFlexStart:
        return crates::layout2::styles::JustifySelf::FlexStart();
      case specified::JustifySelf::Tag::kFlexEnd:
        return crates::layout2::styles::JustifySelf::FlexEnd();
      case specified::JustifySelf::Tag::kStretch:
        return crates::layout2::styles::JustifySelf::Stretch();
      default:
        return crates::layout2::styles::JustifySelf::Start();
      }
    }
  };

  class JustifyItems : public specified::JustifyItems,
                       public ToLayoutValue<crates::layout2::styles::JustifyItems>
  {
    using specified::JustifyItems::JustifyItems;

  public:
    JustifyItems(specified::JustifyItems other) : specified::JustifyItems(other)
    {
    }

    crates::layout2::styles::JustifyItems toLayoutValue() const override
    {
      switch (tag())
      {
      case specified::JustifyItems::Tag::kStart:
        return crates::layout2::styles::JustifyItems::Start();
      case specified::JustifyItems::Tag::kEnd:
        return crates::layout2::styles::JustifyItems::End();
      case specified::JustifyItems::Tag::kCenter:
        return crates::layout2::styles::JustifyItems::Center();
      case specified::JustifyItems::Tag::kFlexStart:
        return crates::layout2::styles::JustifyItems::FlexStart();
      case specified::JustifyItems::Tag::kFlexEnd:
        return crates::layout2::styles::JustifyItems::FlexEnd();
      case specified::JustifyItems::Tag::kStretch:
        return crates::layout2::styles::JustifyItems::Stretch();
      default:
        return crates::layout2::styles::JustifyItems::Start();
      }
    }
  };
}
