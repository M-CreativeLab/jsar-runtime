#pragma once

#include <cstdint>
#include <concepts>
#include <optional>
#include <crates/bindings.hpp>
#include <common/utility.hpp>

namespace client_cssom::values::generics
{
  class PositionType : public ToLayoutValue<crates::layout2::styles::Position>
  {
  protected:
    enum Tag : uint8_t
    {
      kStatic = 0,
      kRelative,
      kAbsolute,
      kFixed,
      kSticky,
    };

  public:
    static PositionType Static() { return PositionType(kStatic); }
    static PositionType Relative() { return PositionType(kRelative); }
    static PositionType Absolute() { return PositionType(kAbsolute); }
    static PositionType Fixed() { return PositionType(kFixed); }
    static PositionType Sticky() { return PositionType(kSticky); }

  public:
    PositionType()
        : tag_(kStatic)
    {
    }

  private:
    PositionType(Tag tag)
        : tag_(tag)
    {
    }

  public:
    inline bool isStatic() const { return tag_ == kStatic; }
    inline bool isRelative() const { return tag_ == kRelative; }
    inline bool isAbsolute() const { return tag_ == kAbsolute; }
    inline bool isFixed() const { return tag_ == kFixed; }
    inline bool isSticky() const { return tag_ == kSticky; }

    inline crates::layout2::styles::Position toLayoutValue() const
    {
      switch (tag_)
      {
      case kStatic:
      case kRelative:
        return crates::layout2::styles::Position::Relative();
      case kAbsolute:
        return crates::layout2::styles::Position::Absolute();
      default:
        // TODO(yorkie): support fixed and sticky.
        return crates::layout2::styles::Position::Relative();
      }
    }

  protected:
    Tag tag_;
  };

  class PositionComponent
  {
    virtual bool isCenter() const = 0;
  };

  template <typename H, typename V>
    requires transmute::common::derived_from<H, PositionComponent> &&
             transmute::common::derived_from<V, PositionComponent>
  class GenericPosition
  {
  public:
    GenericPosition(H horizontal, V vertical)
        : horizontal(horizontal),
          vertical(vertical)
    {
    }

  public:
    bool isCenter() const
    {
      return horizontal.isCenter() && vertical.isCenter();
    }

  public:
    H horizontal;
    V vertical;
  };

  template <typename Pos>
  class GenericPositionOrAuto
  {
  private:
    enum Tag : uint8_t
    {
      kPosition,
      kAuto,
    };

  public:
    static GenericPositionOrAuto<Pos> Position(Pos position)
    {
      return GenericPositionOrAuto<Pos>(kPosition, position);
    }
    static GenericPositionOrAuto<Pos> Auto()
    {
      return GenericPositionOrAuto<Pos>(kAuto);
    }

  private:
    GenericPositionOrAuto(Tag tag, std::optional<Pos> position = std::nullopt)
        : tag_(tag)
    {
    }

  public:
    inline bool isAuto() const { return tag_ == kAuto; }
    inline Pos getPosition() const
    {
      assert(tag_ == kPosition && "The tag is not position.");
      return position_.value();
    }

  private:
    Tag tag_;
    std::optional<Pos> position_ = std::nullopt;
  };
}
