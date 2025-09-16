#pragma once

#include <cstdint>
#include <client/cssom/style_traits.hpp>

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

  private:
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

  class WhiteSpace : public Parse,
                     public ToCss
  {
    friend class Parse;

  protected:
    enum Tag : uint8_t
    {
      kNormal,
      kNowrap,
      kPre,
      kPreLine,
      kPreWrap,
      kBreakSpaces,
    };

  public:
    static WhiteSpace Normal()
    {
      return WhiteSpace(Tag::kNormal);
    }
    static WhiteSpace Nowrap()
    {
      return WhiteSpace(Tag::kNowrap);
    }
    static WhiteSpace Pre()
    {
      return WhiteSpace(Tag::kPre);
    }
    static WhiteSpace PreLine()
    {
      return WhiteSpace(Tag::kPreLine);
    }
    static WhiteSpace PreWrap()
    {
      return WhiteSpace(Tag::kPreWrap);
    }
    static WhiteSpace BreakSpaces()
    {
      return WhiteSpace(Tag::kBreakSpaces);
    }

  public:
    WhiteSpace()
        : tag_(Tag::kNormal)
    {
    }

  private:
    WhiteSpace(Tag tag)
        : tag_(tag)
    {
    }

  private:
    bool parse(const std::string &input) override
    {
      if (input == "normal")
        tag_ = Tag::kNormal;
      else if (input == "nowrap")
        tag_ = Tag::kNowrap;
      else if (input == "pre")
        tag_ = Tag::kPre;
      else if (input == "pre-line")
        tag_ = Tag::kPreLine;
      else if (input == "pre-wrap")
        tag_ = Tag::kPreWrap;
      else if (input == "break-spaces")
        tag_ = Tag::kBreakSpaces;
      else
        return false;
      return true;
    }
    std::string toCss() const override
    {
      switch (tag_)
      {
      case Tag::kNormal:
        return "normal";
      case Tag::kNowrap:
        return "nowrap";
      case Tag::kPre:
        return "pre";
      case Tag::kPreLine:
        return "pre-line";
      case Tag::kPreWrap:
        return "pre-wrap";
      case Tag::kBreakSpaces:
        return "break-spaces";
      }
      assert(false && "Invalid tag.");
    }

  protected:
    Tag tag_ = Tag::kNormal;
  };
}
