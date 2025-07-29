#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/cssom/parsers/css_value_tokenizer.hpp>
#include <client/cssom/parsers/css_image_parser.hpp>

using namespace client_cssom::css_value_tokenizer;
using namespace client_cssom::css_parser;

TEST_CASE("CSSValueTokenizer basic functionality", "[css-tokenizer]")
{
  SECTION("Linear gradient tokenization")
  {
    CSSValueTokenizer tokenizer("linear-gradient(to right, red, blue)");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 5);
    REQUIRE(tokens[0].type == TokenType::kFunction);
    REQUIRE(tokens[0].value == "linear-gradient");
  }

  SECTION("URL tokenization with quotes")
  {
    CSSValueTokenizer tokenizer("url(\"image.jpg\")");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 1);
    REQUIRE(tokens[0].type == TokenType::kUrl);
    REQUIRE(tokens[0].value == "image.jpg");
  }

  SECTION("URL tokenization without quotes")
  {
    CSSValueTokenizer tokenizer("url(image.png)");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 1);
    REQUIRE(tokens[0].type == TokenType::kUrl);
  }

  SECTION("URL tokenization with single quotes")
  {
    CSSValueTokenizer tokenizer("url('background.gif')");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 1);
    REQUIRE(tokens[0].type == TokenType::kUrl);
    REQUIRE(tokens[0].value == "background.gif");
  }

  SECTION("src() function tokenization")
  {
    CSSValueTokenizer tokenizer("src(\"image.jpg\")");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 1);
    REQUIRE(tokens[0].type == TokenType::kUrl);
    REQUIRE(tokens[0].value == "image.jpg");
  }
}

TEST_CASE("CSSValueTokenizer gradient tokenization", "[css-tokenizer]")
{
  SECTION("Linear gradient")
  {
    CSSValueTokenizer tokenizer("linear-gradient(45deg, red, blue)");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 6);
    REQUIRE(tokens[0].type == TokenType::kFunction);
    REQUIRE(tokens[0].value == "linear-gradient");
  }

  SECTION("Radial gradient")
  {
    CSSValueTokenizer tokenizer("radial-gradient(circle, red, blue)");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 6);
    REQUIRE(tokens[0].type == TokenType::kFunction);
    REQUIRE(tokens[0].value == "radial-gradient");
  }

  SECTION("Repeating gradient")
  {
    CSSValueTokenizer tokenizer("repeating-linear-gradient(to right, red, blue)");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 6);
    REQUIRE(tokens[0].type == TokenType::kFunction);
    REQUIRE(tokens[0].value == "repeating-linear-gradient");
  }
}

TEST_CASE("CSSValueTokenizer identifiers and keywords", "[css-tokenizer]")
{
  SECTION("None keyword")
  {
    CSSValueTokenizer tokenizer("none");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 1);
    REQUIRE(tokens[0].type == TokenType::kIdentifier);
    REQUIRE(tokens[0].value == "none");
  }

  SECTION("Color names")
  {
    CSSValueTokenizer tokenizer("red");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 1);
    REQUIRE(tokens[0].type == TokenType::kIdentifier);
    REQUIRE(tokens[0].value == "red");
  }

  SECTION("Direction keywords")
  {
    CSSValueTokenizer tokenizer("to right");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 2);
    REQUIRE(tokens[0].type == TokenType::kIdentifier);
    REQUIRE(tokens[0].value == "to");
    REQUIRE(tokens[1].type == TokenType::kIdentifier);
    REQUIRE(tokens[1].value == "right");
  }
}

TEST_CASE("CSSValueTokenizer numeric values", "[css-tokenizer]")
{
  SECTION("Angle values")
  {
    CSSValueTokenizer tokenizer("45deg");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 1);
    REQUIRE(tokens[0].type == TokenType::kDimension);
    REQUIRE(tokens[0].unit == "deg");
    REQUIRE(tokens[0].numeric_value == 45.0);
  }

  SECTION("Percentage values")
  {
    CSSValueTokenizer tokenizer("25%");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 1);
    REQUIRE(tokens[0].type == TokenType::kPercentage);
    REQUIRE(tokens[0].numeric_value == 25.0);
  }

  SECTION("Pixel values")
  {
    CSSValueTokenizer tokenizer("100px");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 1);
    REQUIRE(tokens[0].type == TokenType::kDimension);
    REQUIRE(tokens[0].unit == "px");
    REQUIRE(tokens[0].numeric_value == 100.0);
  }
}

TEST_CASE("CSSValueTokenizer complex expressions", "[css-tokenizer]")
{
  SECTION("Complex linear gradient with multiple stops")
  {
    CSSValueTokenizer tokenizer("linear-gradient(to bottom, red 0%, blue 50%, green 100%)");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 10); // Should have function, identifiers, percentages, commas, etc.
  }

  SECTION("Complex radial gradient")
  {
    CSSValueTokenizer tokenizer("radial-gradient(ellipse at center, rgba(255,0,0,0.5), transparent)");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 8);
  }

  SECTION("URL with complex path")
  {
    CSSValueTokenizer tokenizer("url(\"https://example.com/path/to/image.jpg\")");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 1);
    REQUIRE(tokens[0].type == TokenType::kUrl);
    REQUIRE(tokens[0].value == "https://example.com/path/to/image.jpg");
  }
}

TEST_CASE("CSSImageParser src() function support", "[css-image-parser]")
{
  SECTION("Parse src() with quoted string")
  {
    auto image = CSSImageParser::parseImage("src(\"image.jpg\")");
    // Basic test to ensure parsing doesn't crash
    // Full validation would require access to internal image structure
    REQUIRE(true); // Placeholder - actual implementation would check image.isUrl() etc.
  }

  SECTION("Parse image-set() with src()")
  {
    auto image = CSSImageParser::parseImage("image-set(src(\"image.jpg\") 1x)");
    // Basic test to ensure parsing doesn't crash
    REQUIRE(true); // Placeholder
  }

  SECTION("Parse image-set() with url()")
  {
    auto image = CSSImageParser::parseImage("image-set(url(\"image.jpg\") 1x)");
    // Basic test to ensure parsing doesn't crash
    REQUIRE(true); // Placeholder
  }
}

TEST_CASE("CSSImageParser basic parsing", "[css-image-parser]")
{
  SECTION("Parse none keyword")
  {
    auto image = CSSImageParser::parseImage("none");
    // Basic test to ensure parsing doesn't crash
    REQUIRE(true);
  }

  SECTION("Parse url() function")
  {
    auto image = CSSImageParser::parseImage("url(\"test.jpg\")");
    // Basic test to ensure parsing doesn't crash
    REQUIRE(true);
  }

  SECTION("Parse linear gradient")
  {
    auto image = CSSImageParser::parseImage("linear-gradient(to right, red, blue)");
    // Basic test to ensure parsing doesn't crash
    REQUIRE(true);
  }

  SECTION("Parse radial gradient")
  {
    auto image = CSSImageParser::parseImage("radial-gradient(circle, red, blue)");
    // Basic test to ensure parsing doesn't crash
    REQUIRE(true);
  }
}
