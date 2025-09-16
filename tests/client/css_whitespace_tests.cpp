#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/cssom/values/specified/text.hpp>
#include <client/cssom/values/computed/text.hpp>

using namespace client_cssom::values;

TEST_CASE("WhiteSpace parsing and conversion", "[css-white-space]")
{
  SECTION("Parse normal")
  {
    specified::WhiteSpace whiteSpace;
    REQUIRE(whiteSpace.parse("normal"));
    REQUIRE(whiteSpace.toCss() == "normal");
  }

  SECTION("Parse nowrap")
  {
    specified::WhiteSpace whiteSpace;
    REQUIRE(whiteSpace.parse("nowrap"));
    REQUIRE(whiteSpace.toCss() == "nowrap");
  }

  SECTION("Parse pre")
  {
    specified::WhiteSpace whiteSpace;
    REQUIRE(whiteSpace.parse("pre"));
    REQUIRE(whiteSpace.toCss() == "pre");
  }

  SECTION("Parse pre-line")
  {
    specified::WhiteSpace whiteSpace;
    REQUIRE(whiteSpace.parse("pre-line"));
    REQUIRE(whiteSpace.toCss() == "pre-line");
  }

  SECTION("Parse pre-wrap")
  {
    specified::WhiteSpace whiteSpace;
    REQUIRE(whiteSpace.parse("pre-wrap"));
    REQUIRE(whiteSpace.toCss() == "pre-wrap");
  }

  SECTION("Parse break-spaces")
  {
    specified::WhiteSpace whiteSpace;
    REQUIRE(whiteSpace.parse("break-spaces"));
    REQUIRE(whiteSpace.toCss() == "break-spaces");
  }

  SECTION("Parse invalid value")
  {
    specified::WhiteSpace whiteSpace;
    REQUIRE_FALSE(whiteSpace.parse("invalid"));
  }

  SECTION("Default value is normal")
  {
    specified::WhiteSpace whiteSpace;
    REQUIRE(whiteSpace.toCss() == "normal");
  }
}

TEST_CASE("WhiteSpace computed value behavior", "[css-white-space-computed]")
{
  SECTION("normal behavior")
  {
    computed::WhiteSpace whiteSpace = computed::WhiteSpace::Normal();
    REQUIRE(whiteSpace.shouldCollapseSpaces());
    REQUIRE(whiteSpace.shouldCollapseNewlines());
    REQUIRE(whiteSpace.shouldWrap());
    REQUIRE_FALSE(whiteSpace.shouldPreserveSpaces());
    REQUIRE_FALSE(whiteSpace.shouldPreserveNewlines());
    REQUIRE_FALSE(whiteSpace.shouldBreakSpaces());
  }

  SECTION("nowrap behavior")
  {
    computed::WhiteSpace whiteSpace = computed::WhiteSpace::Nowrap();
    REQUIRE(whiteSpace.shouldCollapseSpaces());
    REQUIRE(whiteSpace.shouldCollapseNewlines());
    REQUIRE_FALSE(whiteSpace.shouldWrap());
    REQUIRE_FALSE(whiteSpace.shouldPreserveSpaces());
    REQUIRE_FALSE(whiteSpace.shouldPreserveNewlines());
    REQUIRE_FALSE(whiteSpace.shouldBreakSpaces());
  }

  SECTION("pre behavior")
  {
    computed::WhiteSpace whiteSpace = computed::WhiteSpace::Pre();
    REQUIRE_FALSE(whiteSpace.shouldCollapseSpaces());
    REQUIRE_FALSE(whiteSpace.shouldCollapseNewlines());
    REQUIRE_FALSE(whiteSpace.shouldWrap());
    REQUIRE(whiteSpace.shouldPreserveSpaces());
    REQUIRE(whiteSpace.shouldPreserveNewlines());
    REQUIRE_FALSE(whiteSpace.shouldBreakSpaces());
  }

  SECTION("pre-line behavior")
  {
    computed::WhiteSpace whiteSpace = computed::WhiteSpace::PreLine();
    REQUIRE(whiteSpace.shouldCollapseSpaces());
    REQUIRE_FALSE(whiteSpace.shouldCollapseNewlines());
    REQUIRE(whiteSpace.shouldWrap());
    REQUIRE_FALSE(whiteSpace.shouldPreserveSpaces());
    REQUIRE(whiteSpace.shouldPreserveNewlines());
    REQUIRE_FALSE(whiteSpace.shouldBreakSpaces());
  }

  SECTION("pre-wrap behavior")
  {
    computed::WhiteSpace whiteSpace = computed::WhiteSpace::PreWrap();
    REQUIRE_FALSE(whiteSpace.shouldCollapseSpaces());
    REQUIRE_FALSE(whiteSpace.shouldCollapseNewlines());
    REQUIRE(whiteSpace.shouldWrap());
    REQUIRE(whiteSpace.shouldPreserveSpaces());
    REQUIRE(whiteSpace.shouldPreserveNewlines());
    REQUIRE_FALSE(whiteSpace.shouldBreakSpaces());
  }

  SECTION("break-spaces behavior")
  {
    computed::WhiteSpace whiteSpace = computed::WhiteSpace::BreakSpaces();
    REQUIRE_FALSE(whiteSpace.shouldCollapseSpaces());
    REQUIRE_FALSE(whiteSpace.shouldCollapseNewlines());
    REQUIRE(whiteSpace.shouldWrap());
    REQUIRE(whiteSpace.shouldPreserveSpaces());
    REQUIRE(whiteSpace.shouldPreserveNewlines());
    REQUIRE(whiteSpace.shouldBreakSpaces());
  }
}