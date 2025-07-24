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

  SECTION("Parse length values")
  {
    specified::BackgroundSize size;
    REQUIRE(size.parse("100px"));
    REQUIRE(size.isLengthPercentage());
    REQUIRE(size.toCss() == "100px");
  }

  SECTION("Parse percentage values")
  {
    specified::BackgroundSize size;
    REQUIRE(size.parse("50%"));
    REQUIRE(size.isLengthPercentage());
    REQUIRE(size.toCss() == "50%");
  }

  SECTION("Parse em values")
  {
    specified::BackgroundSize size;
    REQUIRE(size.parse("3.2em"));
    REQUIRE(size.isLengthPercentage());
    REQUIRE(size.toCss() == "3.2em");
  }

  SECTION("Parse two-value syntax")
  {
    specified::BackgroundSize size;
    REQUIRE(size.parse("50% auto"));
    REQUIRE(size.isLengthPercentagePair());
    REQUIRE(size.toCss() == "50% auto");
  }

  SECTION("Parse two-value syntax with lengths")
  {
    specified::BackgroundSize size;
    REQUIRE(size.parse("3em 25%"));
    REQUIRE(size.isLengthPercentagePair());
    REQUIRE(size.toCss() == "3em 25%");
  }

  SECTION("Parse two-value syntax with auto")
  {
    specified::BackgroundSize size;
    REQUIRE(size.parse("auto 6px"));
    REQUIRE(size.isLengthPercentagePair());
    REQUIRE(size.toCss() == "auto 6px");
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

  SECTION("Parse percentage values")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("25%"));
    REQUIRE(position.isLengthPercentage());
    REQUIRE(position.toCss() == "25%");
  }

  SECTION("Parse length values")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("10px"));
    REQUIRE(position.isLengthPercentage());
    REQUIRE(position.toCss() == "10px");
  }

  SECTION("Parse em values")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("1em"));
    REQUIRE(position.isLengthPercentage());
    REQUIRE(position.toCss() == "1em");
  }

  SECTION("Parse two-value percentage syntax")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("25% 75%"));
    REQUIRE(position.isLengthPercentagePair());
    REQUIRE(position.toCss() == "25% 75%");
  }

  SECTION("Parse two-value length syntax")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("0 0"));
    REQUIRE(position.isLengthPercentagePair());
    REQUIRE(position.toCss() == "0px 0px");
  }

  SECTION("Parse mixed keyword and length")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("center 10px"));
    REQUIRE(position.isLengthPercentagePair());
    REQUIRE(position.toCss() == "50% 10px");
  }

  SECTION("Parse cm values")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("1cm 2cm"));
    REQUIRE(position.isLengthPercentagePair());
    REQUIRE(position.toCss() == "1cm 2cm");
  }

  SECTION("Parse ch values")
  {
    specified::BackgroundPosition position;
    REQUIRE(position.parse("10ch 8em"));
    REQUIRE(position.isLengthPercentagePair());
    REQUIRE(position.toCss() == "10ch 8em");
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
    
    // Test length/percentage values
    size.parse("100px");
    computed::BackgroundSize computedLength = size.toComputedValue(context);
    REQUIRE(computedLength.isLengthPercentage());
    
    size.parse("50%");
    computed::BackgroundSize computedPercent = size.toComputedValue(context);
    REQUIRE(computedPercent.isLengthPercentage());
    
    size.parse("100px 50%");
    computed::BackgroundSize computedPair = size.toComputedValue(context);
    REQUIRE(computedPair.isLengthPercentagePair());
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
    
    // Test length/percentage values
    position.parse("25%");
    computed::BackgroundPosition computedPercent = position.toComputedValue(context);
    REQUIRE(computedPercent.isLengthPercentage());
    
    position.parse("10px");
    computed::BackgroundPosition computedLength = position.toComputedValue(context);
    REQUIRE(computedLength.isLengthPercentage());
    
    position.parse("25% 75%");
    computed::BackgroundPosition computedPair = position.toComputedValue(context);
    REQUIRE(computedPair.isLengthPercentagePair());
  }
}

TEST_CASE("Advanced BackgroundSize parsing", "[css-background-size-advanced]")
{
  SECTION("Various length units")
  {
    specified::BackgroundSize size;
    
    REQUIRE(size.parse("12px"));
    REQUIRE(size.isLengthPercentage());
    
    REQUIRE(size.parse("3.2em"));
    REQUIRE(size.isLengthPercentage());
    
    REQUIRE(size.parse("1cm"));
    REQUIRE(size.isLengthPercentage());
    
    REQUIRE(size.parse("10ch"));
    REQUIRE(size.isLengthPercentage());
  }
  
  SECTION("Mixed two-value syntax")
  {
    specified::BackgroundSize size;
    
    REQUIRE(size.parse("50% auto"));
    REQUIRE(size.isLengthPercentagePair());
    REQUIRE(size.toCss() == "50% auto");
    
    REQUIRE(size.parse("3em 25%"));
    REQUIRE(size.isLengthPercentagePair());
    REQUIRE(size.toCss() == "3em 25%");
    
    REQUIRE(size.parse("auto 6px"));
    REQUIRE(size.isLengthPercentagePair());
    REQUIRE(size.toCss() == "auto 6px");
    
    REQUIRE(size.parse("auto auto"));
    REQUIRE(size.isLengthPercentagePair());
    REQUIRE(size.toCss() == "auto auto");
  }
}

TEST_CASE("Advanced BackgroundPosition parsing", "[css-background-position-advanced]")
{
  SECTION("Various length units")
  {
    specified::BackgroundPosition position;
    
    REQUIRE(position.parse("0"));
    REQUIRE(position.isLengthPercentage());
    
    REQUIRE(position.parse("1cm"));
    REQUIRE(position.isLengthPercentage());
    
    REQUIRE(position.parse("10ch"));
    REQUIRE(position.isLengthPercentage());
    
    REQUIRE(position.parse("8em"));
    REQUIRE(position.isLengthPercentage());
  }
  
  SECTION("Two-value combinations")
  {
    specified::BackgroundPosition position;
    
    REQUIRE(position.parse("0 0"));
    REQUIRE(position.isLengthPercentagePair());
    REQUIRE(position.toCss() == "0px 0px");
    
    REQUIRE(position.parse("1cm 2cm"));
    REQUIRE(position.isLengthPercentagePair());
    REQUIRE(position.toCss() == "1cm 2cm");
    
    REQUIRE(position.parse("10ch 8em"));
    REQUIRE(position.isLengthPercentagePair());
    REQUIRE(position.toCss() == "10ch 8em");
  }
  
  SECTION("Keyword to percentage conversion")
  {
    specified::BackgroundPosition position;
    
    REQUIRE(position.parse("left center"));
    REQUIRE(position.isLengthPercentagePair());
    REQUIRE(position.toCss() == "0% 50%");
    
    REQUIRE(position.parse("right bottom"));
    REQUIRE(position.isLengthPercentagePair());
    REQUIRE(position.toCss() == "100% 100%");
    
    REQUIRE(position.parse("center top"));
    REQUIRE(position.isLengthPercentagePair());
    REQUIRE(position.toCss() == "50% 0%");
  }
}

TEST_CASE("BackgroundSize edge cases", "[css-background-size-edge]")
{
  SECTION("Parse multiple valid values")
  {
    specified::BackgroundSize size;
    
    // Single length values
    REQUIRE(size.parse("100px"));
    REQUIRE(size.isLengthPercentage());
    
    REQUIRE(size.parse("50%"));
    REQUIRE(size.isLengthPercentage());
    
    // Two-value syntax
    REQUIRE(size.parse("auto auto"));
    REQUIRE(size.isLengthPercentagePair());
    
    REQUIRE(size.parse("100px 50%"));
    REQUIRE(size.isLengthPercentagePair());
  }
  
  SECTION("Parse invalid values")
  {
    specified::BackgroundSize size;
    REQUIRE_FALSE(size.parse(""));
    REQUIRE_FALSE(size.parse("invalid"));
    REQUIRE_FALSE(size.parse("100px 50% 25%")); // Too many values
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
    
    size.parse("100px");
    REQUIRE(size.toCss() == "100px");
    
    size.parse("50%");
    REQUIRE(size.toCss() == "50%");
    
    size.parse("auto auto");
    REQUIRE(size.toCss() == "auto auto");
  }
}

TEST_CASE("BackgroundPosition edge cases", "[css-background-position-edge]")
{
  SECTION("Parse multiple valid values")
  {
    specified::BackgroundPosition position;
    
    // Single values
    REQUIRE(position.parse("100px"));
    REQUIRE(position.isLengthPercentage());
    
    REQUIRE(position.parse("50%"));
    REQUIRE(position.isLengthPercentage());
    
    // Two-value syntax
    REQUIRE(position.parse("left top"));
    REQUIRE(position.isLengthPercentagePair());
    
    REQUIRE(position.parse("center center"));
    REQUIRE(position.isLengthPercentagePair());
    
    REQUIRE(position.parse("100px 50%"));
    REQUIRE(position.isLengthPercentagePair());
  }
  
  SECTION("Parse invalid values")
  {
    specified::BackgroundPosition position;
    REQUIRE_FALSE(position.parse(""));
    REQUIRE_FALSE(position.parse("invalid"));
    REQUIRE_FALSE(position.parse("left top center")); // Too many values
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
    
    position.parse("50%");
    REQUIRE(position.toCss() == "50%");
    
    position.parse("25% 75%");
    REQUIRE(position.toCss() == "25% 75%");
  }
}
