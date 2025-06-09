#pragma once

#include <cstdint>
#include <string>
#include <ostream>
#include <optional>
#include <client/cssom/style_traits.hpp>

namespace client_cssom::values::specified
{
  class AlignFlags : public ToCss
  {
  public:
    enum Tag : uint8_t
    {
      kAuto = 0,
      kNormal,
      kStart,
      kEnd,
      kFlexStart,
      kFlexEnd,
      kCenter,
      kLeft,
      kRight,
      kBaseline,
      kFirstBaseline,
      kLastBaseline,
      kStretch,
      kSelfStart,
      kSelfEnd,
      kSpaceBetween,
      kSpaceAround,
      kSpaceEvenly,
      kAnchorCenter,
    };

    // Additional flags stored in the upper bits.
    static constexpr uint8_t LEGACY = 1 << 5;
    static constexpr uint8_t SAFE = 1 << 6;
    static constexpr uint8_t UNSAFE = 1 << 7;

    // Mask for the above additional flags.
    static constexpr uint8_t ADDITIONAL_MASK = 0b11100000;

  public:
    AlignFlags(Tag tag = kAuto, std::optional<uint8_t> extra_flags = std::nullopt)
    {
      set(tag, extra_flags);
    }

  public:
    inline void set(Tag tag, std::optional<uint8_t> extra_flags = std::nullopt)
    {
      flag_bits_ = static_cast<uint8_t>(tag);
      if (extra_flags.has_value())
        flag_bits_ |= (*extra_flags & ADDITIONAL_MASK);
    }
    inline Tag tag() const { return static_cast<Tag>(flag_bits_ & ~ADDITIONAL_MASK); }
    inline uint8_t extraFlags() const { return flag_bits_ & ADDITIONAL_MASK; }
    inline bool isLegacy() const { return extraFlags() == LEGACY; }
    inline bool isSafe() const { return extraFlags() == SAFE; }
    inline bool isUnsafe() const { return extraFlags() == UNSAFE; }

    std::string toCss() const override
    {
      std::stringstream ss;
      auto extra_flags = extraFlags();
      if (extra_flags == LEGACY)
        ss << "legacy ";
      else if (extra_flags == SAFE)
        ss << "safe ";
      else if (extra_flags == UNSAFE)
        ss << "unsafe ";

      switch (tag())
      {
      case kAuto:
        ss << "auto";
        break;
      case kNormal:
        ss << "normal";
        break;
      case kStart:
        ss << "start";
        break;
      case kEnd:
        ss << "end";
        break;
      case kFlexStart:
        ss << "flex-start";
        break;
      case kFlexEnd:
        ss << "flex-end";
        break;
      case kCenter:
        ss << "center";
        break;
      case kLeft:
        ss << "left";
        break;
      case kRight:
        ss << "right";
        break;
      case kBaseline:
        ss << "baseline";
        break;
      case kFirstBaseline:
        ss << "first-baseline";
        break;
      case kLastBaseline:
        ss << "last-baseline";
        break;
      case kStretch:
        ss << "stretch";
        break;
      case kSelfStart:
        ss << "self-start";
        break;
      case kSelfEnd:
        ss << "self-end";
        break;
      case kSpaceBetween:
        ss << "space-between";
        break;
      case kSpaceAround:
        ss << "space-around";
        break;
      case kSpaceEvenly:
        ss << "space-evenly";
        break;
      case kAnchorCenter:
        ss << "anchor-center";
        break;
      default:
        ss << "unknown";
        break;
      }
      return ss.str();
    }

  private:
    uint8_t flag_bits_;
  };

  enum class AxisDirection
  {
    // Block direction
    kBlock,
    // Inline direction
    kInline,
  };

  template <typename T, AxisDirection axis>
  class ContentDistribution : public AlignFlags,
                              public Parse
  {
    friend class Parse;
    using AlignFlags::AlignFlags;

  public:
    static T Normal() { return T(kNormal); }

  protected:
    bool parse(const std::string &input) override
    {
      if (axis == AxisDirection::kBlock)
      {
        if (input == "baseline")
        {
          set(kBaseline);
          return true;
        }
        else if (input == "first baseline")
        {
          set(kFirstBaseline);
          return true;
        }
        else if (input == "last baseline")
        {
          set(kLastBaseline);
          return true;
        }
      }
      else
      {
        if (input == "left")
        {
          set(kLeft);
          return true;
        }
        else if (input == "right")
        {
          set(kRight);
          return true;
        }
      }

      // Basic keywords
      if (input == "normal")
        set(kNormal);
      else if (input == "stretch")
        set(kStretch);
      // Positional alignment
      else if (input == "center")
        set(kCenter);
      else if (input == "start")
        set(kStart);
      else if (input == "end")
        set(kEnd);
      else if (input == "flex-start")
        set(kFlexStart);
      else if (input == "flex-end")
        set(kFlexEnd);
      // Distributed alignment
      else if (input == "space-between")
        set(kSpaceBetween);
      else if (input == "space-around")
        set(kSpaceAround);
      else if (input == "space-evenly")
        set(kSpaceEvenly);
      // Overflow alignment
      else if (input == "safe center")
        set(kCenter, SAFE);
      else if (input == "unsafe center")
        set(kCenter, UNSAFE);

      return true;
    }
  };

  class AlignContent : public ContentDistribution<AlignContent, AxisDirection::kBlock>
  {
    using ContentDistribution::ContentDistribution;
  };

  class JustifyContent : public ContentDistribution<JustifyContent, AxisDirection::kInline>
  {
    using ContentDistribution::ContentDistribution;
  };

  template <typename T>
  class SelfAlignment : public AlignFlags,
                        public Parse
  {
    friend class Parse;
    using AlignFlags::AlignFlags;

  public:
    static T Auto() { return T(kAuto); }

  protected:
    bool parse(const std::string &input) override
    {
      // Base keywords
      if (input == "auto")
        set(kAuto);
      else if (input == "normal")
        set(kNormal);
      // Positional alignment
      else if (input == "center")
        set(kCenter);
      else if (input == "start")
        set(kStart);
      else if (input == "end")
        set(kEnd);
      else if (input == "flex-start")
        set(kFlexStart);
      else if (input == "flex-end")
        set(kFlexEnd);
      else if (input == "self-start")
        set(kSelfStart);
      else if (input == "self-end")
        set(kSelfEnd);
      else if (input == "anchor-center")
        set(kAnchorCenter);
      // Baseline alignment
      else if (input == "baseline")
        set(kBaseline);
      else if (input == "first baseline")
        set(kFirstBaseline);
      else if (input == "last baseline")
        set(kLastBaseline);
      else if (input == "stretch")
        set(kStretch);
      // Overflow alignment
      else if (input == "safe center")
        set(kCenter, SAFE);
      else if (input == "unsafe center")
        set(kCenter, UNSAFE);

      return true;
    }

    // Returns whether this value is valid for both axis directions.
    bool isValidOnBothAxes() const
    {
      switch (tag())
      {
      case kLeft:
      case kRight:
        return false;
      default:
        return true;
      }
    }
  };

  class AlignSelf : public SelfAlignment<AlignSelf>
  {
    using SelfAlignment::SelfAlignment;
  };

  class JustifySelf : public SelfAlignment<JustifySelf>
  {
    friend class Parse;
    using SelfAlignment::SelfAlignment;

  private:
    bool parse(const std::string &input) override
    {
      if (input == "left")
      {
        set(kLeft);
        return true;
      }
      else if (input == "right")
      {
        set(kRight);
        return true;
      }
      return SelfAlignment::parse(input);
    }
  };

  class AlignItems : public AlignFlags,
                     public Parse
  {
    friend class Parse;
    using AlignFlags::AlignFlags;

  public:
    static AlignItems Normal() { return AlignItems(kNormal); }

  private:
    bool parse(const std::string &input) override
    {
      // Basic keywords
      if (input == "normal")
        set(kNormal);
      else if (input == "stretch")
        set(kStretch);
      // Positional alignment
      else if (input == "center")
        set(kCenter);
      else if (input == "start")
        set(kStart);
      else if (input == "end")
        set(kEnd);
      else if (input == "flex-start")
        set(kFlexStart);
      else if (input == "flex-end")
        set(kFlexEnd);
      else if (input == "self-start")
        set(kSelfStart);
      else if (input == "self-end")
        set(kSelfEnd);
      else if (input == "anchor-center")
        set(kAnchorCenter);
      // Baseline alignment
      else if (input == "baseline")
        set(kBaseline);
      else if (input == "first-baseline")
        set(kFirstBaseline);
      else if (input == "last-baseline")
        set(kLastBaseline);
      // Overflow alignment
      else if (input == "safe center")
        set(kCenter, SAFE);
      else if (input == "unsafe center")
        set(kCenter, UNSAFE);

      return true;
    }
  };

  class JustifyItems : public AlignFlags,
                       public Parse
  {
    friend class Parse;
    using AlignFlags::AlignFlags;

  public:
    static JustifyItems Legacy() { return JustifyItems(kAuto, LEGACY); }
    static JustifyItems Normal() { return JustifyItems(kNormal); }

  private:
    bool parse(const std::string &input) override
    {
      // Basic keywords
      if (input == "normal")
        set(kNormal);
      else if (input == "stretch")
        set(kStretch);
      // Positional alignment
      else if (input == "center")
        set(kCenter);
      else if (input == "start")
        set(kStart);
      else if (input == "end")
        set(kEnd);
      else if (input == "flex-start")
        set(kFlexStart);
      else if (input == "flex-end")
        set(kFlexEnd);
      else if (input == "self-start")
        set(kSelfStart);
      else if (input == "self-end")
        set(kSelfEnd);
      else if (input == "left")
        set(kLeft);
      else if (input == "right")
        set(kRight);
      else if (input == "anchor-center")
        set(kAnchorCenter);
      // Baseline alignment
      else if (input == "baseline")
        set(kBaseline);
      else if (input == "first-baseline")
        set(kFirstBaseline);
      else if (input == "last-baseline")
        set(kLastBaseline);
      // Overflow alignment
      else if (input == "safe center")
        set(kCenter, SAFE);
      else if (input == "unsafe center")
        set(kCenter, UNSAFE);
      // Legacy alignment
      else if (input == "legacy right")
        set(kRight, LEGACY);
      else if (input == "legacy left")
        set(kLeft, LEGACY);
      else if (input == "legacy center")
        set(kCenter, LEGACY);

      return true;
    }
  };
}
