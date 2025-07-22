#pragma once

#include <cstdint>
#include <client/cssom/style_traits.hpp>

namespace client_cssom::values::specified
{
  class BackgroundClip : public Parse,
                         public ToCss,
                         public ToComputedValue<BackgroundClip>
  {
    friend class Parse;

  private:
    enum Tag : uint8_t
    {
      kBorderBox = 0,
      kPaddingBox,
      kContentBox,
      kText,
    };

  public:
    static BackgroundClip BorderBox()
    {
      return BackgroundClip(kBorderBox);
    }
    static BackgroundClip PaddingBox()
    {
      return BackgroundClip(kPaddingBox);
    }
    static BackgroundClip ContentBox()
    {
      return BackgroundClip(kContentBox);
    }
    static BackgroundClip Text()
    {
      return BackgroundClip(kText);
    }

  public:
    BackgroundClip()
        : tag_(kBorderBox)
    {
    }

  private:
    BackgroundClip(Tag tag)
        : tag_(tag)
    {
    }

    bool parse(const std::string &input) override
    {
      if (input == "border-box")
        tag_ = kBorderBox;
      else if (input == "padding-box")
        tag_ = kPaddingBox;
      else if (input == "content-box")
        tag_ = kContentBox;
      else if (input == "text")
        tag_ = kText;
      return true;
    }

  public:
    std::string toCss() const override
    {
      switch (tag_)
      {
      case kBorderBox:
        return "border-box";
      case kPaddingBox:
        return "padding-box";
      case kContentBox:
        return "content-box";
      case kText:
        return "text";
      }
      return "";
    }
    BackgroundClip toComputedValue(computed::Context &) const override
    {
      return *this;
    }

    inline bool isBorderBox() const
    {
      return tag_ == kBorderBox;
    }
    inline bool isPaddingBox() const
    {
      return tag_ == kPaddingBox;
    }
    inline bool isContentBox() const
    {
      return tag_ == kContentBox;
    }
    inline bool isText() const
    {
      return tag_ == kText;
    }

  private:
    Tag tag_;
  };
}