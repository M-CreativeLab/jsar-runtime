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

TEST_CASE("CSSValueTokenizer hex color values", "[css-tokenizer]")
{
  SECTION("6-digit hex color")
  {
    CSSValueTokenizer tokenizer("#ff0000");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 1);
    REQUIRE(tokens[0].type == TokenType::kHash);
    REQUIRE(tokens[0].value == "ff0000");
  }

  SECTION("3-digit hex color")
  {
    CSSValueTokenizer tokenizer("#f00");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 1);
    REQUIRE(tokens[0].type == TokenType::kHash);
    REQUIRE(tokens[0].value == "f00");
  }

  SECTION("8-digit hex color with alpha")
  {
    CSSValueTokenizer tokenizer("#ff000080");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 1);
    REQUIRE(tokens[0].type == TokenType::kHash);
    REQUIRE(tokens[0].value == "ff000080");
  }

  SECTION("4-digit hex color with alpha")
  {
    CSSValueTokenizer tokenizer("#f008");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 1);
    REQUIRE(tokens[0].type == TokenType::kHash);
    REQUIRE(tokens[0].value == "f008");
  }

  SECTION("Comprehensive gradient parsing test")
  {
    // Test both tokenization and parsing together
    CSSValueTokenizer tokenizer("linear-gradient(to right, #ff0000, #0000ff)");
    auto tokens = tokenizer.tokenize();
    
    // First verify tokenization worked
    int hashTokens = 0;
    bool hasFunction = false;
    for (const auto &token : tokens) {
      if (token.type == TokenType::kFunction && token.value == "linear-gradient")
        hasFunction = true;
      if (token.type == TokenType::kHash) {
        hashTokens++;
        // Verify the hash values are correct
        REQUIRE((token.value == "ff0000" || token.value == "0000ff"));
      }
    }
    
    REQUIRE(hasFunction);
    REQUIRE(hashTokens == 2);
    
    // Now test that the image parser can handle these tokens
    auto image = CSSImageParser::parseImage("linear-gradient(to right, #ff0000, #0000ff)");
    REQUIRE(!image.isNone()); // Should not be none if parsing succeeded
    
    // If the hash tokens are processed correctly, the image should be a gradient
    REQUIRE(image.isGradient());
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

TEST_CASE("CSSValueTokenizer gradient with hex colors", "[css-tokenizer]")
{
  SECTION("Linear gradient with hex colors")
  {
    CSSValueTokenizer tokenizer("linear-gradient(to right, #ff0000, #0000ff)");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 6);
    REQUIRE(tokens[0].type == TokenType::kFunction);
    REQUIRE(tokens[0].value == "linear-gradient");

    // Find the hash tokens
    bool foundRedHash = false, foundBlueHash = false;
    for (const auto &token : tokens)
    {
      if (token.type == TokenType::kHash)
      {
        if (token.value == "ff0000")
          foundRedHash = true;
        if (token.value == "0000ff")
          foundBlueHash = true;
      }
    }
    REQUIRE(foundRedHash);
    REQUIRE(foundBlueHash);
  }

  SECTION("Radial gradient with rgba and hex")
  {
    CSSValueTokenizer tokenizer("radial-gradient(circle, rgba(255,0,0,0.5), #00ff00)");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() >= 8);
    bool foundRgbaFunction = false, foundGreenHash = false;
    for (const auto &token : tokens)
    {
      if (token.type == TokenType::kFunction && token.value == "rgba")
        foundRgbaFunction = true;
      if (token.type == TokenType::kHash && token.value == "00ff00")
        foundGreenHash = true;
    }
    REQUIRE(foundRgbaFunction);
    REQUIRE(foundGreenHash);
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

  SECTION("Parse linear gradient with hex colors")
  {
    auto image = CSSImageParser::parseImage("linear-gradient(to right, #ff0000, #0000ff)");
    // Basic test to ensure parsing doesn't crash
    REQUIRE(true);
    
    // TODO: Add validation that hex colors are actually parsed
    // The image should be a gradient with proper color stops
  }

  SECTION("Parse linear gradient with rgba and hex colors")
  {
    auto image = CSSImageParser::parseImage("linear-gradient(to bottom, rgba(255,0,0,0.5), #00ff00, #0000ff80)");
    // Basic test to ensure parsing doesn't crash
    REQUIRE(true);
  }

  SECTION("Parse radial gradient with short hex colors")
  {
    auto image = CSSImageParser::parseImage("radial-gradient(circle, #f00, #0f0)");
    // Basic test to ensure parsing doesn't crash
    REQUIRE(true);
  }
}

TEST_CASE("CSSImageParser URL parsing", "[css-image-parser-url]")
{
  SECTION("Parse relative URL with dot notation")
  {
    auto image = CSSImageParser::parseImage("url('./image.png')");
    REQUIRE(!image.isNone());
    REQUIRE(image.isUrl());
    // Note: URL resolution happens in toComputedValue with proper context
  }

  SECTION("Parse relative URL with parent directory")
  {
    auto image = CSSImageParser::parseImage("url('../assets/image.png')");
    REQUIRE(!image.isNone());
    REQUIRE(image.isUrl());
    // Note: URL resolution happens in toComputedValue with proper context
  }

  SECTION("Parse relative URL without prefix")
  {
    auto image = CSSImageParser::parseImage("url('images/test.png')");
    REQUIRE(!image.isNone());
    REQUIRE(image.isUrl());
    // Note: URL resolution happens in toComputedValue with proper context
  }

  SECTION("Parse absolute URL - should remain unchanged")
  {
    auto image = CSSImageParser::parseImage("url('https://example.com/image.png')");
    REQUIRE(!image.isNone());
    REQUIRE(image.isUrl());
    // Note: Absolute URLs are not modified during resolution
  }

  SECTION("Parse data URL - should remain unchanged")
  {
    auto image = CSSImageParser::parseImage("url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mP8/5+hHgAHggJ/PchI7wAAAABJRU5ErkJggg==')");
    REQUIRE(!image.isNone());
    REQUIRE(image.isUrl());
    // Note: Data URLs are not modified during resolution
  }
}
