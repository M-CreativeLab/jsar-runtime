#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/cssom/values/specified/background.hpp>

using namespace client_cssom::values;

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
  SECTION("Parse left")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("left"));
    REQUIRE(position.isLeft());
    REQUIRE(position.toCss() == "left");
  }

  SECTION("Parse center")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("center"));
    REQUIRE(position.isCenter());
    REQUIRE(position.toCss() == "center");
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

  SECTION("Parse invalid value")
  {
    specified::BackgroundPosition position;
    REQUIRE_FALSE(position.parse("invalid-value"));
  }

  SECTION("Default value")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.isCenter());
  }
}
