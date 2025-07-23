#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/cssom/values/specified/background.hpp>
#include <client/cssom/values/computed/background.hpp>
#include <client/cssom/values/computed/context.hpp>

using namespace client_cssom::values;

TEST_CASE("BackgroundOrigin parsing and conversion", "[css-background-origin]")
{
  SECTION("Parse padding-box")
  {
    specified::BackgroundOrigin origin;
    REQUIRE(origin.parse("padding-box"));
    REQUIRE(origin.isPaddingBox());

    auto context = computed::Context::From(nullptr);
    auto computed_origin = origin.toComputedValue(context);
    REQUIRE(computed_origin.isPaddingBox());
    REQUIRE(computed_origin.toCss() == "padding-box");
  }

  SECTION("Parse border-box")
  {
    specified::BackgroundOrigin origin;
    REQUIRE(origin.parse("border-box"));
    REQUIRE(origin.isBorderBox());

    auto context = computed::Context::From(nullptr);
    auto computed_origin = origin.toComputedValue(context);
    REQUIRE(computed_origin.isBorderBox());
    REQUIRE(computed_origin.toCss() == "border-box");
  }

  SECTION("Parse content-box")
  {
    specified::BackgroundOrigin origin;
    REQUIRE(origin.parse("content-box"));
    REQUIRE(origin.isContentBox());

    auto context = computed::Context::From(nullptr);
    auto computed_origin = origin.toComputedValue(context);
    REQUIRE(computed_origin.isContentBox());
    REQUIRE(computed_origin.toCss() == "content-box");
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

    auto context = computed::Context::From(nullptr);
    auto computed_repeat = repeat.toComputedValue(context);
    REQUIRE(computed_repeat.isRepeat());
    REQUIRE(computed_repeat.toCss() == "repeat");
  }

  SECTION("Parse repeat-x")
  {
    specified::BackgroundRepeat repeat;
    REQUIRE(repeat.parse("repeat-x"));
    REQUIRE(repeat.isRepeatX());

    auto context = computed::Context::From(nullptr);
    auto computed_repeat = repeat.toComputedValue(context);
    REQUIRE(computed_repeat.isRepeatX());
    REQUIRE(computed_repeat.toCss() == "repeat-x");
  }

  SECTION("Parse repeat-y")
  {
    specified::BackgroundRepeat repeat;
    REQUIRE(repeat.parse("repeat-y"));
    REQUIRE(repeat.isRepeatY());

    auto context = computed::Context::From(nullptr);
    auto computed_repeat = repeat.toComputedValue(context);
    REQUIRE(computed_repeat.isRepeatY());
    REQUIRE(computed_repeat.toCss() == "repeat-y");
  }

  SECTION("Parse no-repeat")
  {
    specified::BackgroundRepeat repeat;
    REQUIRE(repeat.parse("no-repeat"));
    REQUIRE(repeat.isNoRepeat());

    auto context = computed::Context::From(nullptr);
    auto computed_repeat = repeat.toComputedValue(context);
    REQUIRE(computed_repeat.isNoRepeat());
    REQUIRE(computed_repeat.toCss() == "no-repeat");
  }

  SECTION("Parse space")
  {
    specified::BackgroundRepeat repeat;
    REQUIRE(repeat.parse("space"));
    REQUIRE(repeat.isSpace());

    auto context = computed::Context::From(nullptr);
    auto computed_repeat = repeat.toComputedValue(context);
    REQUIRE(computed_repeat.isSpace());
    REQUIRE(computed_repeat.toCss() == "space");
  }

  SECTION("Parse round")
  {
    specified::BackgroundRepeat repeat;
    REQUIRE(repeat.parse("round"));
    REQUIRE(repeat.isRound());

    auto context = computed::Context::From(nullptr);
    auto computed_repeat = repeat.toComputedValue(context);
    REQUIRE(computed_repeat.isRound());
    REQUIRE(computed_repeat.toCss() == "round");
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