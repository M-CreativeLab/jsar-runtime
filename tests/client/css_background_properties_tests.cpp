#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/cssom/values/specified/background.hpp>
#include <client/cssom/values/computed/background.hpp>

using namespace client_cssom::values::specified;
using namespace client_cssom::values::computed;

TEST_CASE("BackgroundOrigin parsing and conversion", "[css-background-origin]")
{
  SECTION("Parse padding-box")
  {
    BackgroundOrigin origin;
    REQUIRE(origin.parse("padding-box"));
    REQUIRE(origin.isPaddingBox());
    
    computed::Context context;
    auto computed_origin = origin.toComputedValue(context);
    REQUIRE(computed_origin.isPaddingBox());
    REQUIRE(computed_origin.toCss() == "padding-box");
  }

  SECTION("Parse border-box")
  {
    BackgroundOrigin origin;
    REQUIRE(origin.parse("border-box"));
    REQUIRE(origin.isBorderBox());
    
    computed::Context context;
    auto computed_origin = origin.toComputedValue(context);
    REQUIRE(computed_origin.isBorderBox());
    REQUIRE(computed_origin.toCss() == "border-box");
  }

  SECTION("Parse content-box")
  {
    BackgroundOrigin origin;
    REQUIRE(origin.parse("content-box"));
    REQUIRE(origin.isContentBox());
    
    computed::Context context;
    auto computed_origin = origin.toComputedValue(context);
    REQUIRE(computed_origin.isContentBox());
    REQUIRE(computed_origin.toCss() == "content-box");
  }

  SECTION("Parse invalid value")
  {
    BackgroundOrigin origin;
    REQUIRE_FALSE(origin.parse("invalid-value"));
  }

  SECTION("Default value")
  {
    BackgroundOrigin origin;
    REQUIRE(origin.isPaddingBox());
  }
}

TEST_CASE("BackgroundRepeat parsing and conversion", "[css-background-repeat]")
{
  SECTION("Parse repeat")
  {
    BackgroundRepeat repeat;
    REQUIRE(repeat.parse("repeat"));
    REQUIRE(repeat.isRepeat());
    
    computed::Context context;
    auto computed_repeat = repeat.toComputedValue(context);
    REQUIRE(computed_repeat.isRepeat());
    REQUIRE(computed_repeat.toCss() == "repeat");
  }

  SECTION("Parse repeat-x")
  {
    BackgroundRepeat repeat;
    REQUIRE(repeat.parse("repeat-x"));
    REQUIRE(repeat.isRepeatX());
    
    computed::Context context;
    auto computed_repeat = repeat.toComputedValue(context);
    REQUIRE(computed_repeat.isRepeatX());
    REQUIRE(computed_repeat.toCss() == "repeat-x");
  }

  SECTION("Parse repeat-y")
  {
    BackgroundRepeat repeat;
    REQUIRE(repeat.parse("repeat-y"));
    REQUIRE(repeat.isRepeatY());
    
    computed::Context context;
    auto computed_repeat = repeat.toComputedValue(context);
    REQUIRE(computed_repeat.isRepeatY());
    REQUIRE(computed_repeat.toCss() == "repeat-y");
  }

  SECTION("Parse no-repeat")
  {
    BackgroundRepeat repeat;
    REQUIRE(repeat.parse("no-repeat"));
    REQUIRE(repeat.isNoRepeat());
    
    computed::Context context;
    auto computed_repeat = repeat.toComputedValue(context);
    REQUIRE(computed_repeat.isNoRepeat());
    REQUIRE(computed_repeat.toCss() == "no-repeat");
  }

  SECTION("Parse space")
  {
    BackgroundRepeat repeat;
    REQUIRE(repeat.parse("space"));
    REQUIRE(repeat.isSpace());
    
    computed::Context context;
    auto computed_repeat = repeat.toComputedValue(context);
    REQUIRE(computed_repeat.isSpace());
    REQUIRE(computed_repeat.toCss() == "space");
  }

  SECTION("Parse round")
  {
    BackgroundRepeat repeat;
    REQUIRE(repeat.parse("round"));
    REQUIRE(repeat.isRound());
    
    computed::Context context;
    auto computed_repeat = repeat.toComputedValue(context);
    REQUIRE(computed_repeat.isRound());
    REQUIRE(computed_repeat.toCss() == "round");
  }

  SECTION("Parse invalid value")
  {
    BackgroundRepeat repeat;
    REQUIRE_FALSE(repeat.parse("invalid-value"));
  }

  SECTION("Default value")
  {
    BackgroundRepeat repeat;
    REQUIRE(repeat.isRepeat());
  }
}