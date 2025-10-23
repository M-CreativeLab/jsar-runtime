#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>
#include <cassert>
#include <client/cssom/style_traits.hpp>

namespace endor
{
  namespace client_cssom::values::specified
  {
    class TextAlign : public Parse,
                      public ToCss
    {
      friend class Parse;

    protected:
      enum Tag : uint8_t
      {
        kStart,
        kEnd,
        kLeft,
        kRight,
        kCenter,
        kJustify,
        kMatchParent,
      };

    public:
      static TextAlign Start()
      {
        return TextAlign(Tag::kStart);
      }
      static TextAlign End()
      {
        return TextAlign(Tag::kEnd);
      }
      static TextAlign Left()
      {
        return TextAlign(Tag::kLeft);
      }
      static TextAlign Right()
      {
        return TextAlign(Tag::kRight);
      }
      static TextAlign Center()
      {
        return TextAlign(Tag::kCenter);
      }
      static TextAlign Justify()
      {
        return TextAlign(Tag::kJustify);
      }
      static TextAlign MatchParent()
      {
        return TextAlign(Tag::kMatchParent);
      }

    public:
      TextAlign()
          : tag_(Tag::kStart)
      {
      }

    protected:
      TextAlign(Tag tag)
          : tag_(tag)
      {
      }

    private:
      bool parse(const std::string &input) override
      {
        if (input == "start")
          tag_ = Tag::kStart;
        else if (input == "end")
          tag_ = Tag::kEnd;
        else if (input == "left")
          tag_ = Tag::kLeft;
        else if (input == "right")
          tag_ = Tag::kRight;
        else if (input == "center")
          tag_ = Tag::kCenter;
        else if (input == "justify")
          tag_ = Tag::kJustify;
        else if (input == "match-parent")
          tag_ = Tag::kMatchParent;
        return true;
      }
      std::string toCss() const override
      {
        switch (tag_)
        {
        case Tag::kStart:
          return "start";
        case Tag::kEnd:
          return "end";
        case Tag::kLeft:
          return "left";
        case Tag::kRight:
          return "right";
        case Tag::kCenter:
          return "center";
        case Tag::kJustify:
          return "justify";
        case Tag::kMatchParent:
          return "match-parent";
        }
        assert(false && "Invalid tag.");
      }

    protected:
      Tag tag_ = Tag::kStart;
    };

    class Direction : public Parse,
                      public ToCss
    {
      friend class Parse;

    protected:
      enum Tag
      {
        kLTR,
        kRTL,
      };

    public:
      static Direction LTR()
      {
        return Direction(true);
      }
      static Direction RTL()
      {
        return Direction(false);
      }

    public:
      Direction()
          : tag_(Tag::kLTR)
      {
      }

    private:
      Direction(bool isLTR)
          : tag_(isLTR ? Tag::kLTR : Tag::kRTL)
      {
      }

    private:
      bool parse(const std::string &input) override
      {
        tag_ = input == "ltr" ? Tag::kLTR : Tag::kRTL;
        return true;
      }

    public:
      std::string toCss() const override
      {
        return tag_ == Tag::kLTR ? "ltr" : "rtl";
      }

    protected:
      Tag tag_ = Tag::kLTR;
    };

    class VerticalAlign : public Parse,
                          public ToCss
    {
      friend class Parse;

    public:
      enum Tag : uint8_t
      {
        kBaseline,
        kSub,
        kSuper,
        kTop,
        kTextTop,
        kMiddle,
        kBottom,
        kTextBottom,
        kLength,     // Custom length value
        kPercentage, // Percentage value
      };

    public:
      static VerticalAlign Baseline()
      {
        return VerticalAlign(Tag::kBaseline);
      }
      static VerticalAlign Sub()
      {
        return VerticalAlign(Tag::kSub);
      }
      static VerticalAlign Super()
      {
        return VerticalAlign(Tag::kSuper);
      }
      static VerticalAlign Top()
      {
        return VerticalAlign(Tag::kTop);
      }
      static VerticalAlign TextTop()
      {
        return VerticalAlign(Tag::kTextTop);
      }
      static VerticalAlign Middle()
      {
        return VerticalAlign(Tag::kMiddle);
      }
      static VerticalAlign Bottom()
      {
        return VerticalAlign(Tag::kBottom);
      }
      static VerticalAlign TextBottom()
      {
        return VerticalAlign(Tag::kTextBottom);
      }
      static VerticalAlign Length(float value)
      {
        return VerticalAlign(Tag::kLength, value);
      }
      static VerticalAlign Percentage(float value)
      {
        return VerticalAlign(Tag::kPercentage, value);
      }

    public:
      VerticalAlign()
          : tag_(Tag::kBaseline)
          , value_(0.0f)
      {
      }

    protected:
      VerticalAlign(Tag tag, float value = 0.0f)
          : tag_(tag)
          , value_(value)
      {
      }

    public:
      Tag tag() const
      {
        return tag_;
      }
      float value() const
      {
        return value_;
      }
      std::string toCss() const override
      {
        switch (tag_)
        {
        case Tag::kBaseline:
          return "baseline";
        case Tag::kSub:
          return "sub";
        case Tag::kSuper:
          return "super";
        case Tag::kTop:
          return "top";
        case Tag::kTextTop:
          return "text-top";
        case Tag::kMiddle:
          return "middle";
        case Tag::kBottom:
          return "bottom";
        case Tag::kTextBottom:
          return "text-bottom";
        case Tag::kLength:
          return std::to_string(value_) + "px";
        case Tag::kPercentage:
          return std::to_string(value_) + "%";
        }
        assert(false && "Invalid tag.");
      }

    private:
      bool parse(const std::string &input) override
      {
        if (input == "baseline")
          tag_ = Tag::kBaseline;
        else if (input == "sub")
          tag_ = Tag::kSub;
        else if (input == "super")
          tag_ = Tag::kSuper;
        else if (input == "top")
          tag_ = Tag::kTop;
        else if (input == "text-top")
          tag_ = Tag::kTextTop;
        else if (input == "middle")
          tag_ = Tag::kMiddle;
        else if (input == "bottom")
          tag_ = Tag::kBottom;
        else if (input == "text-bottom")
          tag_ = Tag::kTextBottom;
        else
        {
          // Try to parse as length or percentage
          if (input.back() == '%')
          {
            try
            {
              float percentage = std::stof(input.substr(0, input.size() - 1));
              tag_ = Tag::kPercentage;
              value_ = percentage;
            }
            catch (const std::exception &)
            {
              return false;
            }
          }
          else if (input.find("px") != std::string::npos)
          {
            try
            {
              float length = std::stof(input.substr(0, input.size() - 2));
              tag_ = Tag::kLength;
              value_ = length;
            }
            catch (const std::exception &)
            {
              return false;
            }
          }
          else
          {
            return false;
          }
        }
        return true;
      }

    protected:
      Tag tag_ = Tag::kBaseline;
      float value_ = 0.0f;
    };
  }
} // namespace endor
