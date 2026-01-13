#include "../catch2/catch_amalgamated.hpp"
#include <client/cssom/parsers/css_tokenizer.hpp>

using namespace client_cssom::css_tokenizer;

TEST_CASE("CSS Tokenizer Tests", "[css-tokenizer]")
{
  SECTION("Basic tokens")
  {
    CSSTokenizer tokenizer("body { color: red; }");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens.size() == 8); // body, whitespace, {, whitespace, color, :, whitespace, red, ;, whitespace, }, EOF
    REQUIRE(tokens[0].type == TokenType::kIdentifier);
    REQUIRE(tokens[0].value == "body");
    REQUIRE(tokens[2].type == TokenType::kLeftCurlyBracket);
    REQUIRE(tokens[4].type == TokenType::kIdentifier);
    REQUIRE(tokens[4].value == "color");
    REQUIRE(tokens[5].type == TokenType::kColon);
    REQUIRE(tokens[7].type == TokenType::kIdentifier);
    REQUIRE(tokens[7].value == "red");
  }

  SECTION("At-keywords")
  {
    CSSTokenizer tokenizer("@media screen and (max-width: 768px)");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens[0].type == TokenType::kAtKeyword);
    REQUIRE(tokens[0].value == "@media");
    REQUIRE(tokens[2].type == TokenType::kIdentifier);
    REQUIRE(tokens[2].value == "screen");
  }

  SECTION("Hash tokens")
  {
    CSSTokenizer tokenizer("#main { color: #ff0000; }");
    auto tokens = tokenizer.tokenize();

    bool found_main_hash = false;
    bool found_color_hash = false;
    
    for (const auto &token : tokens)
    {
      if (token.type == TokenType::kHash)
      {
        if (token.value == "#main")
          found_main_hash = true;
        else if (token.value == "#ff0000")
          found_color_hash = true;
      }
    }
    
    REQUIRE(found_main_hash);
    REQUIRE(found_color_hash);
  }

  SECTION("String tokens")
  {
    CSSTokenizer tokenizer("font-family: \"Arial\", 'Times New Roman';");
    auto tokens = tokenizer.tokenize();

    bool found_arial = false;
    bool found_times = false;
    
    for (const auto &token : tokens)
    {
      if (token.type == TokenType::kString)
      {
        if (token.value == "Arial")
          found_arial = true;
        else if (token.value == "Times New Roman")
          found_times = true;
      }
    }
    
    REQUIRE(found_arial);
    REQUIRE(found_times);
  }

  SECTION("Number and dimension tokens")
  {
    CSSTokenizer tokenizer("width: 100px; height: 50%; opacity: 0.5;");
    auto tokens = tokenizer.tokenize();

    bool found_dimension = false;
    bool found_percentage = false;
    bool found_number = false;
    
    for (const auto &token : tokens)
    {
      if (token.type == TokenType::kDimension && token.value == "100px")
      {
        found_dimension = true;
        REQUIRE(token.numeric_value == 100.0);
        REQUIRE(token.unit == "px");
      }
      else if (token.type == TokenType::kPercentage && token.value == "50%")
      {
        found_percentage = true;
        REQUIRE(token.numeric_value == 50.0);
      }
      else if (token.type == TokenType::kNumber && token.value == "0.5")
      {
        found_number = true;
        REQUIRE(token.numeric_value == 0.5);
      }
    }
    
    REQUIRE(found_dimension);
    REQUIRE(found_percentage);
    REQUIRE(found_number);
  }

  SECTION("Function tokens")
  {
    CSSTokenizer tokenizer("background: url(image.jpg); transform: rotate(45deg);");
    auto tokens = tokenizer.tokenize();

    bool found_url = false;
    bool found_rotate = false;
    
    for (const auto &token : tokens)
    {
      if (token.type == TokenType::kUrl && token.value == "image.jpg")
        found_url = true;
      else if (token.type == TokenType::kFunction && token.value == "rotate")
        found_rotate = true;
    }
    
    REQUIRE(found_url);
    REQUIRE(found_rotate);
  }

  SECTION("Comments are filtered out")
  {
    CSSTokenizer tokenizer("/* This is a comment */ body { color: red; }");
    auto tokens = tokenizer.tokenize();

    // Comments should be filtered out during tokenization
    for (const auto &token : tokens)
    {
      REQUIRE(token.type != TokenType::kComment);
    }
    
    // Should still have the body rule tokens
    REQUIRE(tokens[0].type == TokenType::kIdentifier);
    REQUIRE(tokens[0].value == "body");
  }

  SECTION("Complex selector tokens")
  {
    CSSTokenizer tokenizer("div.class#id > p[attr=\"value\"] + h1::before");
    auto tokens = tokenizer.tokenize();

    // Check for various combinator and selector tokens
    bool found_div = false;
    bool found_class = false;
    bool found_id = false;
    bool found_greater = false;
    bool found_bracket = false;
    bool found_attr = false;
    bool found_plus = false;
    
    for (const auto &token : tokens)
    {
      if (token.type == TokenType::kIdentifier && token.value == "div")
        found_div = true;
      else if (token.type == TokenType::kDelimiter && token.value == ".")
        found_class = true;
      else if (token.type == TokenType::kHash && token.value == "#id")
        found_id = true;
      else if (token.type == TokenType::kDelimiter && token.value == ">")
        found_greater = true;
      else if (token.type == TokenType::kLeftSquareBracket)
        found_bracket = true;
      else if (token.type == TokenType::kIdentifier && token.value == "attr")
        found_attr = true;
      else if (token.type == TokenType::kDelimiter && token.value == "+")
        found_plus = true;
    }
    
    REQUIRE(found_div);
    REQUIRE(found_class);
    REQUIRE(found_id);
    REQUIRE(found_greater);
    REQUIRE(found_bracket);
    REQUIRE(found_attr);
    REQUIRE(found_plus);
  }

  SECTION("Attribute selector match tokens")
  {
    CSSTokenizer tokenizer("[attr~=\"value\"] [attr|=\"value\"] [attr^=\"value\"] [attr$=\"value\"] [attr*=\"value\"]");
    auto tokens = tokenizer.tokenize();

    bool found_include_match = false;
    bool found_dash_match = false;
    bool found_prefix_match = false;
    bool found_suffix_match = false;
    bool found_substring_match = false;
    
    for (const auto &token : tokens)
    {
      if (token.type == TokenType::kIncludeMatch)
        found_include_match = true;
      else if (token.type == TokenType::kDashMatch)
        found_dash_match = true;
      else if (token.type == TokenType::kPrefixMatch)
        found_prefix_match = true;
      else if (token.type == TokenType::kSuffixMatch)
        found_suffix_match = true;
      else if (token.type == TokenType::kSubstringMatch)
        found_substring_match = true;
    }
    
    REQUIRE(found_include_match);
    REQUIRE(found_dash_match);
    REQUIRE(found_prefix_match);
    REQUIRE(found_suffix_match);
    REQUIRE(found_substring_match);
  }

  SECTION("CDO and CDC tokens")
  {
    CSSTokenizer tokenizer("<!-- body { color: red; } -->");
    auto tokens = tokenizer.tokenize();

    REQUIRE(tokens[0].type == TokenType::kCDO);
    REQUIRE(tokens[0].value == "<!--");
    
    // Find CDC token
    bool found_cdc = false;
    for (const auto &token : tokens)
    {
      if (token.type == TokenType::kCDC)
      {
        found_cdc = true;
        REQUIRE(token.value == "-->");
        break;
      }
    }
    REQUIRE(found_cdc);
  }

  SECTION("Whitespace handling")
  {
    CSSTokenizer tokenizer("body\n\t{\n  color:  red;\n}");
    auto tokens = tokenizer.tokenize();

    // Should have whitespace tokens preserved
    bool found_whitespace = false;
    for (const auto &token : tokens)
    {
      if (token.type == TokenType::kWhitespace)
      {
        found_whitespace = true;
        break;
      }
    }
    REQUIRE(found_whitespace);
  }

  SECTION("Bad string and URL handling")
  {
    CSSTokenizer tokenizer("content: \"unclosed string; background: url(unclosed-url;");
    auto tokens = tokenizer.tokenize();

    bool found_bad_string = false;
    bool found_bad_url = false;
    
    for (const auto &token : tokens)
    {
      if (token.type == TokenType::kBadString)
        found_bad_string = true;
      else if (token.type == TokenType::kBadUrl)
        found_bad_url = true;
    }
    
    REQUIRE(found_bad_string);
    REQUIRE(found_bad_url);
  }
}