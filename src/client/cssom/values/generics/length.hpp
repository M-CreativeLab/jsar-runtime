#pragma once

#include <optional>

namespace client_cssom::values::generics
{
  template <typename LengthPercent>
  class GenericLengthPercentageOrAuto
  {
  private:
    enum Tag : uint8_t
    {
      kLengthPercentage,
      kAuto,
    };

  public:
    inline bool isAuto() const
    {
      return tag_ == kAuto;
    }
    inline bool isLengthPercentage() const
    {
      return tag_ == kLengthPercentage;
    }

    inline LengthPercent lengthPercent() const
    {
      assert(tag_ == kLengthPercentage && "The tag is not length percentage.");
      return length_percent_.value();
    }

  private:
    Tag tag_;
    std::optional<LengthPercent> length_percent_;
  };

  template <typename T, typename LengthPercent>
  class GenericSize
  {
  private:
    enum Tag : uint8_t
    {
      kLengthPercentage,
      kAuto,
      kMaxContent,
      kMinContent,
      kFitContent,
      kStretch,
    };

  public:
    static T Auto()
    {
      return T(kAuto);
    }
    static T MaxContent()
    {
      return T(kMaxContent);
    }
    static T MinContent()
    {
      return T(kMinContent);
    }
    static T FitContent()
    {
      return T(kFitContent);
    }
    static T Stretch()
    {
      return T(kStretch);
    }
    static T LengthPercentage(LengthPercent length_percent)
    {
      return T(kLengthPercentage, length_percent);
    }

  public:
    GenericSize()
        : tag_(kAuto)
        , length_percent_(std::nullopt)
    {
    }
    GenericSize(Tag tag, std::optional<LengthPercent> length_percent = std::nullopt)
        : tag_(tag)
        , length_percent_(length_percent)
    {
    }

    inline bool isAuto() const
    {
      return tag_ == kAuto;
    }
    inline bool isLengthPercentage() const
    {
      return tag_ == kLengthPercentage;
    }
    inline bool isMaxContent() const
    {
      return tag_ == kMaxContent;
    }
    inline bool isMinContent() const
    {
      return tag_ == kMinContent;
    }
    inline bool isFitContent() const
    {
      return tag_ == kFitContent;
    }
    inline bool isStretch() const
    {
      return tag_ == kStretch;
    }

    inline LengthPercent lengthPercent() const
    {
      assert(tag_ == kLengthPercentage && "The tag is not length percentage.");
      return length_percent_.value();
    }

  protected:
    void setAuto()
    {
      tag_ = kAuto;
      length_percent_ = std::nullopt;
    }
    void setLengthPercentage(LengthPercent length_percent)
    {
      tag_ = kLengthPercentage;
      length_percent_ = length_percent;
    }

  private:
    Tag tag_;
    std::optional<LengthPercent> length_percent_;
  };

  template <typename T, typename LengthPercent>
  class GenericMaxSize
  {
  private:
    enum Tag : uint8_t
    {
      kLengthPercentage,
      kNone,
      kMaxContent,
      kMinContent,
      kFitContent,
      kStretch,
    };

  public:
    static T None()
    {
      return T(kNone);
    }
    static T MaxContent()
    {
      return T(kMaxContent);
    }
    static T MinContent()
    {
      return T(kMinContent);
    }
    static T FitContent()
    {
      return T(kFitContent);
    }
    static T Stretch()
    {
      return T(kStretch);
    }

  private:
    GenericMaxSize(Tag tag)
        : tag_(tag)
    {
    }

  public:
    inline bool isNone() const
    {
      return tag_ == kNone;
    }
    inline bool isMaxContent() const
    {
      return tag_ == kMaxContent;
    }
    inline bool isMinContent() const
    {
      return tag_ == kMinContent;
    }
    inline bool isFitContent() const
    {
      return tag_ == kFitContent;
    }
    inline bool isStretch() const
    {
      return tag_ == kStretch;
    }
    inline bool isLengthPercentage() const
    {
      return tag_ == kLengthPercentage;
    }

    inline LengthPercent lengthPercent() const
    {
      assert(tag_ == kLengthPercentage && "The tag is not length percentage.");
      return length_percent_.value();
    }

  private:
    Tag tag_;
    std::optional<LengthPercent> length_percent_;
  };

  template <typename T, typename LP>
  class GenericMargin
  {
  private:
    enum Tag : uint8_t
    {
      kLengthPercentage,
      kAuto,
    };

  public:
    static T Auto()
    {
      return T(kAuto);
    }
    static T LengthPercentage(LP length_percent)
    {
      return T(kLengthPercentage, length_percent);
    }

  public:
    GenericMargin()
        : tag_(kLengthPercentage)
        , length_percent_(LP(0.0f))
    {
    }

  private:
    GenericMargin(Tag tag, std::optional<LP> length_percent = std::nullopt)
        : tag_(tag)
        , length_percent_(length_percent)
    {
    }

  public:
    inline bool isAuto() const
    {
      return tag_ == kAuto;
    }
    inline bool isLengthPercentage() const
    {
      return tag_ == kLengthPercentage;
    }

    inline LP lengthPercent() const
    {
      assert(tag_ == kLengthPercentage && "The tag is not length percentage.");
      return length_percent_.value();
    }

  protected:
    void setAuto()
    {
      tag_ = kAuto;
      length_percent_ = std::nullopt;
    }
    void setLengthPercentage(LP length_percent)
    {
      tag_ = kLengthPercentage;
      length_percent_ = length_percent;
    }

  private:
    Tag tag_;
    std::optional<LP> length_percent_;
  };
}
