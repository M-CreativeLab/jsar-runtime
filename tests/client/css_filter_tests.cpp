#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/cssom/values/specified/filter.hpp>
#include <client/cssom/computed_style.hpp>
#include <client/cssom/css_style_declaration.hpp>

using namespace client_cssom::values;

TEST_CASE("Filter parsing and conversion", "[css-filter]")
{
  SECTION("Parse none")
  {
    specified::Filter filter;
    REQUIRE(filter.parse("none"));
    REQUIRE(filter.isNone());
    REQUIRE(filter.toCss() == "none");
  }

  SECTION("Parse non-empty filter")
  {
    specified::Filter filter;
    REQUIRE(filter.parse("blur(5px)"));
    REQUIRE_FALSE(filter.isNone());
  }

  SECTION("Parse empty value")
  {
    specified::Filter filter;
    REQUIRE_FALSE(filter.parse(""));
  }

  SECTION("Default value")
  {
    specified::Filter filter;
    REQUIRE(filter.isNone());
  }
}

TEST_CASE("FilterFunction parsing and conversion", "[css-filter-function]")
{
  SECTION("Parse none")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("none"));
    REQUIRE(func.isNone());
    REQUIRE(func.toCss() == "none");
  }

  SECTION("Parse blur function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("blur(5px)"));
    REQUIRE(func.isBlur());
    REQUIRE(func.toCss() == "blur");
  }

  SECTION("Parse brightness function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("brightness(150%)"));
    REQUIRE(func.isBrightness());
    REQUIRE(func.toCss() == "brightness");
  }

  SECTION("Parse contrast function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("contrast(200%)"));
    REQUIRE(func.isContrast());
    REQUIRE(func.toCss() == "contrast");
  }

  SECTION("Parse grayscale function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("grayscale(50%)"));
    REQUIRE(func.isGrayscale());
    REQUIRE(func.toCss() == "grayscale");
  }

  SECTION("Parse hue-rotate function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("hue-rotate(90deg)"));
    REQUIRE(func.isHueRotate());
    REQUIRE(func.toCss() == "hue-rotate");
  }

  SECTION("Parse invert function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("invert(75%)"));
    REQUIRE(func.isInvert());
    REQUIRE(func.toCss() == "invert");
  }

  SECTION("Parse opacity function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("opacity(25%)"));
    REQUIRE(func.isOpacity());
    REQUIRE(func.toCss() == "opacity");
  }

  SECTION("Parse saturate function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("saturate(30%)"));
    REQUIRE(func.isSaturate());
    REQUIRE(func.toCss() == "saturate");
  }

  SECTION("Parse sepia function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("sepia(60%)"));
    REQUIRE(func.isSepia());
    REQUIRE(func.toCss() == "sepia");
  }

  SECTION("Parse drop-shadow function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("drop-shadow(16px 16px 20px blue)"));
    REQUIRE(func.isDropShadow());
    REQUIRE(func.toCss() == "drop-shadow");
  }

  SECTION("Parse invalid value")
  {
    specified::FilterFunction func;
    REQUIRE_FALSE(func.parse("invalid-function"));
  }

  SECTION("Default value")
  {
    specified::FilterFunction func;
    REQUIRE(func.isNone());
  }
}

TEST_CASE("ComputedStyle Filter Integration", "[css-computed-style-filter]")
{
  SECTION("Default filter values")
  {
    ComputedStyle style;
    REQUIRE(style.filter().isNone());
    REQUIRE(style.backdropFilter().isNone());
  }

  SECTION("Filter getter methods exist")
  {
    ComputedStyle style;
    // Test that we can call the getter methods without errors
    const auto& filter = style.filter();
    const auto& backdropFilter = style.backdropFilter();
    
    REQUIRE(filter.isNone());
    REQUIRE(backdropFilter.isNone());
  }
}