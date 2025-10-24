#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/cssom/values/specified/background.hpp>

using namespace endor;
using namespace endor::client_cssom::values;

TEST_CASE("BackgroundClip parsing and conversion", "[css-background-clip]")
{
  SECTION("Parse border-box")
  {
    specified::BackgroundClip clip;
    REQUIRE(clip.parse("border-box"));
    REQUIRE(clip.isBorderBox());
    REQUIRE(clip.toCss() == "border-box");
  }

  SECTION("Parse padding-box")
  {
    specified::BackgroundClip clip;
    REQUIRE(clip.parse("padding-box"));
    REQUIRE(clip.isPaddingBox());
    REQUIRE(clip.toCss() == "padding-box");
  }

  SECTION("Parse content-box")
  {
    specified::BackgroundClip clip;
    REQUIRE(clip.parse("content-box"));
    REQUIRE(clip.isContentBox());
    REQUIRE(clip.toCss() == "content-box");
  }

  SECTION("Parse text")
  {
    specified::BackgroundClip clip;
    REQUIRE(clip.parse("text"));
    REQUIRE(clip.isText());
    REQUIRE(clip.toCss() == "text");
  }

  SECTION("Parse invalid value")
  {
    specified::BackgroundClip clip;
    REQUIRE_FALSE(clip.parse("invalid-value"));
  }

  SECTION("Default value")
  {
    specified::BackgroundClip clip;
    REQUIRE(clip.isBorderBox());
  }
}

TEST_CASE("BackgroundOrigin parsing and conversion", "[css-background-origin]")
{
  SECTION("Parse padding-box")
  {
    specified::BackgroundOrigin origin;
    REQUIRE(origin.parse("padding-box"));
    REQUIRE(origin.isPaddingBox());
    REQUIRE(origin.toCss() == "padding-box");
  }

  SECTION("Parse border-box")
  {
    specified::BackgroundOrigin origin;
    REQUIRE(origin.parse("border-box"));
    REQUIRE(origin.isBorderBox());
    REQUIRE(origin.toCss() == "border-box");
  }

  SECTION("Parse content-box")
  {
    specified::BackgroundOrigin origin;
    REQUIRE(origin.parse("content-box"));
    REQUIRE(origin.isContentBox());
    REQUIRE(origin.toCss() == "content-box");
  }

  SECTION("Parse invalid value")
  {
    specified::BackgroundOrigin origin;
    REQUIRE_FALSE(origin.parse("invalid-value"));
  }

  SECTION("Default value")
  {
    specified::BackgroundOrigin origin;
    REQUIRE(origin.isPaddingBox());
  }
}

TEST_CASE("BackgroundRepeat parsing and conversion", "[css-background-repeat]")
{
  SECTION("Parse repeat")
  {
    specified::BackgroundRepeat repeat;
    REQUIRE(repeat.parse("repeat"));
    REQUIRE(repeat.isRepeat());
    REQUIRE(repeat.toCss() == "repeat");
  }

  SECTION("Parse repeat-x")
  {
    specified::BackgroundRepeat repeat;
    REQUIRE(repeat.parse("repeat-x"));
    REQUIRE(repeat.isRepeatX());
    REQUIRE(repeat.toCss() == "repeat-x");
  }

  SECTION("Parse repeat-y")
  {
    specified::BackgroundRepeat repeat;
    REQUIRE(repeat.parse("repeat-y"));
    REQUIRE(repeat.isRepeatY());
    REQUIRE(repeat.toCss() == "repeat-y");
  }

  SECTION("Parse no-repeat")
  {
    specified::BackgroundRepeat repeat;
    REQUIRE(repeat.parse("no-repeat"));
    REQUIRE(repeat.isNoRepeat());
    REQUIRE(repeat.toCss() == "no-repeat");
  }

  SECTION("Parse space")
  {
    specified::BackgroundRepeat repeat;
    REQUIRE(repeat.parse("space"));
    REQUIRE(repeat.isSpace());
    REQUIRE(repeat.toCss() == "space");
  }

  SECTION("Parse round")
  {
    specified::BackgroundRepeat repeat;
    REQUIRE(repeat.parse("round"));
    REQUIRE(repeat.isRound());
    REQUIRE(repeat.toCss() == "round");
  }

  SECTION("Parse invalid value")
  {
    specified::BackgroundRepeat repeat;
    REQUIRE_FALSE(repeat.parse("invalid-value"));
  }

  SECTION("Default value")
  {
    specified::BackgroundRepeat repeat;
    REQUIRE(repeat.isRepeat());
  }
}

TEST_CASE("BackgroundSize parsing and conversion", "[css-background-size]")
{
  SECTION("Parse auto")
  {
    specified::BackgroundSize size;
    REQUIRE(size.parse("auto"));
    REQUIRE(size.isAuto());
    REQUIRE(size.toCss() == "auto");
  }

  SECTION("Parse cover")
  {
    specified::BackgroundSize size;
    REQUIRE(size.parse("cover"));
    REQUIRE(size.isCover());
    REQUIRE(size.toCss() == "cover");
  }

  SECTION("Parse contain")
  {
    specified::BackgroundSize size;
    REQUIRE(size.parse("contain"));
    REQUIRE(size.isContain());
    REQUIRE(size.toCss() == "contain");
  }

  SECTION("Parse length")
  {
    specified::BackgroundSize size;
    REQUIRE(size.parse("100px"));
    REQUIRE(size.isLength());
    REQUIRE(size.getWidth() == 100.0f);
    REQUIRE(size.toCss() == "100.000000px");
  }

  SECTION("Parse percentage")
  {
    specified::BackgroundSize size;
    REQUIRE(size.parse("50%"));
    REQUIRE(size.isPercentage());
    REQUIRE(size.getWidth() == 50.0f);
    REQUIRE(size.toCss() == "50.000000%");
  }

  SECTION("Parse invalid value")
  {
    specified::BackgroundSize size;
    REQUIRE_FALSE(size.parse("invalid-value"));
  }

  SECTION("Default value")
  {
    specified::BackgroundSize size;
    REQUIRE(size.isAuto());
  }
}

TEST_CASE("BackgroundPosition parsing and conversion", "[css-background-position]")
{
  SECTION("Parse center")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("center"));
    REQUIRE(position.isCenter());
    REQUIRE(position.toCss() == "center");
  }

  SECTION("Parse left")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("left"));
    REQUIRE(position.isLeft());
    REQUIRE(position.toCss() == "left");
  }

  SECTION("Parse right")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("right"));
    REQUIRE(position.isRight());
    REQUIRE(position.toCss() == "right");
  }

  SECTION("Parse top")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("top"));
    REQUIRE(position.isTop());
    REQUIRE(position.toCss() == "top");
  }

  SECTION("Parse bottom")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("bottom"));
    REQUIRE(position.isBottom());
    REQUIRE(position.toCss() == "bottom");
  }

  SECTION("Parse length")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("10px"));
    REQUIRE(position.isLength());
    REQUIRE(position.getX() == 10.0f);
    REQUIRE(position.toCss() == "10.000000px");
  }

  SECTION("Parse percentage")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("25%"));
    REQUIRE(position.isPercentage());
    REQUIRE(position.getX() == 25.0f);
    REQUIRE(position.toCss() == "25.000000%");
  }

  SECTION("Parse two values - keywords")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("left top"));
    REQUIRE(position.isTwoValues());
    REQUIRE(position.getX() == 0.0f);
    REQUIRE(position.getY() == 0.0f);
    REQUIRE(position.toCss() == "0.000000px 0.000000px");
  }

  SECTION("Parse two values - center center")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("center center"));
    REQUIRE(position.isTwoValues());
    REQUIRE(position.getX() == 50.0f);
    REQUIRE(position.getY() == 50.0f);
  }

  SECTION("Parse two values - lengths")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("20px 30px"));
    REQUIRE(position.isTwoValues());
    REQUIRE(position.getX() == 20.0f);
    REQUIRE(position.getY() == 30.0f);
    REQUIRE(position.toCss() == "20.000000px 30.000000px");
  }

  SECTION("Parse two values - percentages")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("75% 25%"));
    REQUIRE(position.isTwoValues());
    REQUIRE(position.getX() == 75.0f);
    REQUIRE(position.getY() == 25.0f);
  }

  SECTION("Parse two values - mixed")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("left 50%"));
    REQUIRE(position.isTwoValues());
    REQUIRE(position.getX() == 0.0f);
    REQUIRE(position.getY() == 50.0f);
  }

  SECTION("Parse three values - left offset top")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("left 10px top"));
    REQUIRE(position.isThreeValues());
    REQUIRE(position.getHorizontalOffset() == 10.0f);
    REQUIRE(position.getVerticalOffset() == 0.0f);
    REQUIRE(position.toCss() == "left 10.000000px top");
  }

  SECTION("Parse three values - top offset right")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("top 15px right"));
    REQUIRE(position.isThreeValues());
    REQUIRE(position.getVerticalOffset() == 15.0f);
    REQUIRE(position.getHorizontalOffset() == 0.0f);
  }

  SECTION("Parse four values - left offset top offset")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("left 10px top 20px"));
    REQUIRE(position.isFourValues());
    REQUIRE(position.getHorizontalOffset() == 10.0f);
    REQUIRE(position.getVerticalOffset() == 20.0f);
    REQUIRE(position.toCss() == "left 10.000000px top 20.000000px");
  }

  SECTION("Parse four values - bottom offset right offset")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("bottom 5px right 15px"));
    REQUIRE(position.isFourValues());
    REQUIRE(position.getVerticalOffset() == 5.0f);
    REQUIRE(position.getHorizontalOffset() == 15.0f);
  }

  SECTION("Parse invalid value")
  {
    specified::BackgroundPosition position;
    REQUIRE_FALSE(position.parse("invalid-value"));
  }

  SECTION("Parse invalid two values")
  {
    specified::BackgroundPosition position;
    REQUIRE_FALSE(position.parse("invalid invalid"));
  }

  SECTION("Parse invalid three values")
  {
    specified::BackgroundPosition position;
    REQUIRE_FALSE(position.parse("left top bottom"));  // conflicting keywords
  }

  SECTION("Parse invalid four values")
  {
    specified::BackgroundPosition position;
    REQUIRE_FALSE(position.parse("left right top bottom"));  // conflicting keywords
  }

  SECTION("Parse too many values")
  {
    specified::BackgroundPosition position;
    REQUIRE_FALSE(position.parse("left 10px top 20px center"));
  }

  SECTION("Default value")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.isCenter());
  }
}
