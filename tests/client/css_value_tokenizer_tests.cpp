#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/cssom/parsers/css_value_tokenizer.hpp>

using namespace client_cssom::css_value_tokenizer;

TEST_CASE("CSSValueTokenizer negative number support", "[css-tokenizer]")
{
  SECTION("Parse negative percentage")
  {
    CSSValueTokenizer tokenizer("-50%");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::kPercentage);
    REQUIRE(tokens[0].value == "-50%");
    REQUIRE(tokens[0].numeric_value == -50.0);
  }

  SECTION("Parse negative decimal percentage")
  {
    CSSValueTokenizer tokenizer("-25.5%");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::kPercentage);
    REQUIRE(tokens[0].value == "-25.5%");
    REQUIRE(tokens[0].numeric_value == -25.5);
  }

  SECTION("Parse negative dimension")
  {
    CSSValueTokenizer tokenizer("-100px");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::kDimension);
    REQUIRE(tokens[0].value == "-100px");
    REQUIRE(tokens[0].numeric_value == -100.0);
    REQUIRE(tokens[0].unit == "px");
  }

  SECTION("Parse negative number")
  {
    CSSValueTokenizer tokenizer("-123");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::kNumber);
    REQUIRE(tokens[0].value == "-123");
    REQUIRE(tokens[0].numeric_value == -123.0);
  }

  SECTION("Parse negative decimal number")
  {
    CSSValueTokenizer tokenizer("-45.67");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::kNumber);
    REQUIRE(tokens[0].value == "-45.67");
    REQUIRE(tokens[0].numeric_value == -45.67);
  }

  SECTION("Parse negative em dimension")
  {
    CSSValueTokenizer tokenizer("-0.5em");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() == 1);
    REQUIRE(tokens[0].type == TokenType::kDimension);
    REQUIRE(tokens[0].value == "-0.5em");
    REQUIRE(tokens[0].numeric_value == -0.5);
    REQUIRE(tokens[0].unit == "em");
  }

  SECTION("Parse mixed positive and negative values")
  {
    CSSValueTokenizer tokenizer("50% -25px 100 -0.5");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() == 4);

    // First token: 50%
    REQUIRE(tokens[0].type == TokenType::kPercentage);
    REQUIRE(tokens[0].numeric_value == 50.0);

    // Second token: -25px
    REQUIRE(tokens[1].type == TokenType::kDimension);
    REQUIRE(tokens[1].numeric_value == -25.0);
    REQUIRE(tokens[1].unit == "px");

    // Third token: 100
    REQUIRE(tokens[2].type == TokenType::kNumber);
    REQUIRE(tokens[2].numeric_value == 100.0);

    // Fourth token: -0.5
    REQUIRE(tokens[3].type == TokenType::kNumber);
    REQUIRE(tokens[3].numeric_value == -0.5);
  }

  SECTION("Parse transform function with negative percentage")
  {
    CSSValueTokenizer tokenizer("translateX(-50%)");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() == 3); // function, left-paren(skipped), percentage, right-paren

    // Function name as function
    REQUIRE(tokens[0].type == TokenType::kFunction);
    REQUIRE(tokens[0].value == "translateX");

    // Negative percentage
    REQUIRE(tokens[1].type == TokenType::kPercentage);
    REQUIRE(tokens[1].value == "-50%");
    REQUIRE(tokens[1].numeric_value == -50.0);

    // Right parenthesis
    REQUIRE(tokens[2].type == TokenType::kRightParen);
  }

  SECTION("Ensure regular identifiers still work")
  {
    CSSValueTokenizer tokenizer("auto -webkit-transform none");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() == 3);

    // All should be identifiers
    REQUIRE(tokens[0].type == TokenType::kIdentifier);
    REQUIRE(tokens[0].value == "auto");

    REQUIRE(tokens[1].type == TokenType::kIdentifier);
    REQUIRE(tokens[1].value == "-webkit-transform");

    REQUIRE(tokens[2].type == TokenType::kIdentifier);
    REQUIRE(tokens[2].value == "none");
  }
}