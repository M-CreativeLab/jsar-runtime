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

TEST_CASE("BackgroundSize and BackgroundPosition computed values", "[css-background-computed]")
{
  computed::Context context; // Assuming a default context
  
  SECTION("BackgroundSize computed values")
  {
    specified::BackgroundSize size;
    
    size.parse("auto");
    computed::BackgroundSize computedAuto = size.toComputedValue(context);
    REQUIRE(computedAuto.isAuto());
    
    size.parse("cover");
    computed::BackgroundSize computedCover = size.toComputedValue(context);
    REQUIRE(computedCover.isCover());
    
    size.parse("contain");
    computed::BackgroundSize computedContain = size.toComputedValue(context);
    REQUIRE(computedContain.isContain());
  }
  
  SECTION("BackgroundPosition computed values")
  {
    specified::BackgroundPosition position;
    
    position.parse("left");
    computed::BackgroundPosition computedLeft = position.toComputedValue(context);
    REQUIRE(computedLeft.isLeft());
    
    position.parse("center");
    computed::BackgroundPosition computedCenter = position.toComputedValue(context);
    REQUIRE(computedCenter.isCenter());
    
    position.parse("right");
    computed::BackgroundPosition computedRight = position.toComputedValue(context);
    REQUIRE(computedRight.isRight());
    
    position.parse("top");
    computed::BackgroundPosition computedTop = position.toComputedValue(context);
    REQUIRE(computedTop.isTop());
    
    position.parse("bottom");
    computed::BackgroundPosition computedBottom = position.toComputedValue(context);
    REQUIRE(computedBottom.isBottom());
  }
}

TEST_CASE("BackgroundSize edge cases", "[css-background-size-edge]")
{
  SECTION("Parse multiple invalid values")
  {
    specified::BackgroundSize size;
    REQUIRE_FALSE(size.parse(""));
    REQUIRE_FALSE(size.parse("100px"));  // Not yet supported
    REQUIRE_FALSE(size.parse("50%"));    // Not yet supported
    REQUIRE_FALSE(size.parse("auto auto")); // Two values not yet supported
    REQUIRE_FALSE(size.parse("invalid"));
  }
  
  SECTION("CSS serialization roundtrip")
  {
    specified::BackgroundSize size;
    
    size.parse("auto");
    REQUIRE(size.toCss() == "auto");
    
    size.parse("cover");
    REQUIRE(size.toCss() == "cover");
    
    size.parse("contain");
    REQUIRE(size.toCss() == "contain");
  }
}

TEST_CASE("BackgroundPosition edge cases", "[css-background-position-edge]")
{
  SECTION("Parse multiple invalid values")
  {
    specified::BackgroundPosition position;
    REQUIRE_FALSE(position.parse(""));
    REQUIRE_FALSE(position.parse("100px"));  // Not yet supported
    REQUIRE_FALSE(position.parse("50%"));    // Not yet supported
    REQUIRE_FALSE(position.parse("left top")); // Two values not yet supported
    REQUIRE_FALSE(position.parse("invalid"));
  }
  
  SECTION("CSS serialization roundtrip")
  {
    specified::BackgroundPosition position;
    
    position.parse("left");
    REQUIRE(position.toCss() == "left");
    
    position.parse("center");
    REQUIRE(position.toCss() == "center");
    
    position.parse("right");
    REQUIRE(position.toCss() == "right");
    
    position.parse("top");
    REQUIRE(position.toCss() == "top");
    
    position.parse("bottom");
    REQUIRE(position.toCss() == "bottom");
  }
}
