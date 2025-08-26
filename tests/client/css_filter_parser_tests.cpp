#include "../catch2/catch_amalgamated.hpp"
#include <client/cssom/parsers/css_filter_parser.hpp>

using namespace client_cssom::css_filter_parser;

TEST_CASE("CSS Filter Parser Tests", "[css-filter-parser]")
{
  SECTION("Parse none")
  {
    CSSFilterParser parser("none");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.empty());
  }

  SECTION("Parse blur function")
  {
    CSSFilterParser parser("blur(5px)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == FilterFunctionType::kBlur);
    REQUIRE(functions[0].values.size() == 1);
    REQUIRE(functions[0].values[0] == 5.0);
    REQUIRE(functions[0].units[0] == "px");
  }

  SECTION("Parse brightness percentage")
  {
    CSSFilterParser parser("brightness(150%)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == FilterFunctionType::kBrightness);
    REQUIRE(functions[0].values.size() == 1);
    REQUIRE(functions[0].values[0] == 1.5); // 150% converted to 1.5
    REQUIRE(functions[0].units[0] == "%");
  }

  SECTION("Parse hue-rotate with angle")
  {
    CSSFilterParser parser("hue-rotate(90deg)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == FilterFunctionType::kHueRotate);
    REQUIRE(functions[0].values.size() == 1);
    REQUIRE(functions[0].values[0] == 90.0);
    REQUIRE(functions[0].units[0] == "deg");
  }

  SECTION("Parse multiple functions")
  {
    CSSFilterParser parser("blur(5px) brightness(1.2)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 2);
    
    REQUIRE(functions[0].type == FilterFunctionType::kBlur);
    REQUIRE(functions[0].values[0] == 5.0);
    REQUIRE(functions[0].units[0] == "px");
    
    REQUIRE(functions[1].type == FilterFunctionType::kBrightness);
    REQUIRE(functions[1].values[0] == 1.2);
  }

  SECTION("Parse drop-shadow")
  {
    CSSFilterParser parser("drop-shadow(16px 16px 20px blue)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == FilterFunctionType::kDropShadow);
    REQUIRE(functions[0].raw_value == "16px 16px 20px blue");
  }

  SECTION("Parse invalid function")
  {
    CSSFilterParser parser("invalid-function(10px)");
    auto functions = parser.parse();
    
    REQUIRE_FALSE(parser.isValid());
  }

  SECTION("Empty input")
  {
    CSSFilterParser parser("");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.empty());
  }
}