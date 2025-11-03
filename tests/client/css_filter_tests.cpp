#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/cssom/values/specified/filter.hpp>
#include <client/cssom/computed_style.hpp>
#include <client/cssom/css_style_declaration.hpp>

using namespace endor;
using namespace endor::client_cssom::values;

TEST_CASE("Filter parsing and conversion", "[css-filter]")
{
  SECTION("Parse none")
  {
    specified::Filter filter;
    REQUIRE(filter.parse("none"));
    REQUIRE(filter.isNone());
    REQUIRE(filter.toCss() == "none");
  }

  SECTION("Parse filter with multiple functions")
  {
    specified::Filter filter;
    REQUIRE(filter.parse("blur(5px) brightness(1.2)"));
    REQUIRE_FALSE(filter.isNone());
    REQUIRE(filter.toCss() == "blur(5px) brightness(1.2)");

    const auto &functions = filter.getFunctions();
    REQUIRE(functions.size() == 2);
    REQUIRE(functions[0].isBlur());
    REQUIRE(functions[1].isBrightness());
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
  SECTION("Parse blur function with parameters")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("blur(5px)"));
    REQUIRE(func.isBlur());
    REQUIRE(func.toCss() == "blur(5px)");

    const auto &params = func.getParameters();
    REQUIRE(params.size() == 1);
    REQUIRE(params[0].value == 5.0);
    REQUIRE(params[0].unit == "px");
  }

  SECTION("Parse brightness with percentage")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("brightness(150%)"));
    REQUIRE(func.isBrightness());
    REQUIRE(func.toCss() == "brightness(150%)");

    const auto &params = func.getParameters();
    REQUIRE(params.size() == 1);
    REQUIRE(params[0].value == 1.5); // 150% converted to 1.5
    REQUIRE(params[0].unit == "%");
  }

  SECTION("Parse contrast function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("contrast(200%)"));
    REQUIRE(func.isContrast());
    REQUIRE(func.toCss() == "contrast(200%)");
  }

  SECTION("Parse grayscale function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("grayscale(50%)"));
    REQUIRE(func.isGrayscale());
    REQUIRE(func.toCss() == "grayscale(50%)");
  }

  SECTION("Parse hue-rotate function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("hue-rotate(90deg)"));
    REQUIRE(func.isHueRotate());
    REQUIRE(func.toCss() == "hue-rotate(90deg)");
  }

  SECTION("Parse invert function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("invert(75%)"));
    REQUIRE(func.isInvert());
    REQUIRE(func.toCss() == "invert(75%)");
  }

  SECTION("Parse opacity function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("opacity(25%)"));
    REQUIRE(func.isOpacity());
    REQUIRE(func.toCss() == "opacity(25%)");
  }

  SECTION("Parse saturate function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("saturate(30%)"));
    REQUIRE(func.isSaturate());
    REQUIRE(func.toCss() == "saturate(30%)");
  }

  SECTION("Parse sepia function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("sepia(60%)"));
    REQUIRE(func.isSepia());
    REQUIRE(func.toCss() == "sepia(60%)");
  }

  SECTION("Parse drop-shadow function")
  {
    specified::FilterFunction func;
    REQUIRE(func.parse("drop-shadow(16px 16px 20px blue)"));
    REQUIRE(func.isDropShadow());
    REQUIRE(func.toCss() == "drop-shadow(16px 16px 20px blue)");
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
    client_cssom::ComputedStyle style;
    REQUIRE(style.filter().isNone());
    REQUIRE(style.backdropFilter().isNone());
  }

  SECTION("Filter getter methods exist")
  {
    client_cssom::ComputedStyle style;
    // Test that we can call the getter methods without errors
    const auto &filter = style.filter();
    const auto &backdropFilter = style.backdropFilter();

    REQUIRE(filter.isNone());
    REQUIRE(backdropFilter.isNone());
  }
}
