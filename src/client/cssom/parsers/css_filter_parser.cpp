#include "./css_filter_parser.hpp"
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cctype>

namespace client_cssom::css_filter_parser
{
  using namespace std;
  using namespace css_value_tokenizer;

  CSSFilterParser::CSSFilterParser(const string &input)
      : input_(input)
      , tokenizer_(input)
      , current_token_index_(0)
      , is_valid_(false)
  {
    tokens_ = tokenizer_.tokenize();
  }

  vector<FilterFunction> CSSFilterParser::parse()
  {
    vector<FilterFunction> functions;
    current_token_index_ = 0;
    is_valid_ = true;
    error_message_.clear();

    // Handle 'none' case
    if (tokens_.size() == 1 && tokens_[0].type == TokenType::kIdentifier && tokens_[0].value == "none")
    {
      return functions; // Return empty list for 'none'
    }

    // Parse filter functions
    while (!isAtEnd())
    {
      auto func = parseFilterFunction();
      if (!func.has_value())
      {
        is_valid_ = false;
        return functions;
      }
      functions.push_back(func.value());

      skipSeparators();
    }

    return functions;
  }

  optional<FilterFunction> CSSFilterParser::parseFilterFunction()
  {
    if (isAtEnd() || currentToken().type != TokenType::kFunction)
    {
      setError("Expected filter function");
      return nullopt;
    }

    string function_name = currentToken().value;
    FilterFunctionType type = getFunctionType(function_name);

    if (type == FilterFunctionType::kNone)
    {
      setError("Unknown filter function: " + function_name);
      return nullopt;
    }

    advance(); // Consume function token

    // Parse based on function type
    switch (type)
    {
    case FilterFunctionType::kBlur:
      return parseBlur();
    case FilterFunctionType::kBrightness:
      return parseBrightness();
    case FilterFunctionType::kContrast:
      return parseContrast();
    case FilterFunctionType::kDropShadow:
      return parseDropShadow();
    case FilterFunctionType::kGrayscale:
      return parseGrayscale();
    case FilterFunctionType::kHueRotate:
      return parseHueRotate();
    case FilterFunctionType::kInvert:
      return parseInvert();
    case FilterFunctionType::kOpacity:
      return parseOpacity();
    case FilterFunctionType::kSaturate:
      return parseSaturate();
    case FilterFunctionType::kSepia:
      return parseSepia();
    default:
      setError("Unsupported filter function");
      return nullopt;
    }
  }

  optional<FilterFunction> CSSFilterParser::parseBlur()
  {
    FilterFunction func(FilterFunctionType::kBlur);

    // blur() with no arguments defaults to 0px
    if (!isAtEnd() && currentToken().type == TokenType::kRightParen)
    {
      advance(); // Consume closing paren
      func.values.push_back(0.0);
      func.units.push_back("px");
      return func;
    }

    // Parse length value
    double value;
    string unit;
    if (!consumeLength(value, unit))
    {
      setError("Expected length value for blur()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis for blur()");
      return nullopt;
    }

    return func;
  }

  optional<FilterFunction> CSSFilterParser::parseBrightness()
  {
    FilterFunction func(FilterFunctionType::kBrightness);

    // brightness() with no arguments defaults to 1
    if (!isAtEnd() && currentToken().type == TokenType::kRightParen)
    {
      advance(); // Consume closing paren
      func.values.push_back(1.0);
      func.units.push_back("");
      return func;
    }

    // Parse number or percentage
    double value;
    string unit;
    if (consumePercentage(value))
    {
      func.values.push_back(value / 100.0); // Convert percentage to decimal
      func.units.push_back("%");
    }
    else if (consumeNumber(value, unit))
    {
      func.values.push_back(value);
      func.units.push_back(unit);
    }
    else
    {
      setError("Expected number or percentage for brightness()");
      return nullopt;
    }

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis for brightness()");
      return nullopt;
    }

    return func;
  }

  optional<FilterFunction> CSSFilterParser::parseContrast()
  {
    FilterFunction func(FilterFunctionType::kContrast);

    // contrast() with no arguments defaults to 1
    if (!isAtEnd() && currentToken().type == TokenType::kRightParen)
    {
      advance(); // Consume closing paren
      func.values.push_back(1.0);
      func.units.push_back("");
      return func;
    }

    // Parse number or percentage
    double value;
    string unit;
    if (consumePercentage(value))
    {
      func.values.push_back(value / 100.0); // Convert percentage to decimal
      func.units.push_back("%");
    }
    else if (consumeNumber(value, unit))
    {
      func.values.push_back(value);
      func.units.push_back(unit);
    }
    else
    {
      setError("Expected number or percentage for contrast()");
      return nullopt;
    }

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis for contrast()");
      return nullopt;
    }

    return func;
  }

  optional<FilterFunction> CSSFilterParser::parseDropShadow()
  {
    FilterFunction func(FilterFunctionType::kDropShadow);

    // For drop-shadow, we'll store the raw value since it's complex
    // drop-shadow(offset-x offset-y blur-radius color)
    string raw_params;
    int paren_depth = 0;

    while (!isAtEnd())
    {
      const auto &token = currentToken();
      if (token.type == TokenType::kLeftParen)
      {
        paren_depth++;
        raw_params += token.value;
      }
      else if (token.type == TokenType::kRightParen)
      {
        if (paren_depth == 0)
        {
          advance(); // Consume the closing paren
          break;
        }
        paren_depth--;
        raw_params += token.value;
      }
      else
      {
        if (!raw_params.empty())
          raw_params += " ";
        raw_params += token.value;
      }
      advance();
    }

    func.raw_value = raw_params;
    return func;
  }

  optional<FilterFunction> CSSFilterParser::parseGrayscale()
  {
    FilterFunction func(FilterFunctionType::kGrayscale);

    // grayscale() with no arguments defaults to 1
    if (!isAtEnd() && currentToken().type == TokenType::kRightParen)
    {
      advance(); // Consume closing paren
      func.values.push_back(1.0);
      func.units.push_back("");
      return func;
    }

    // Parse number or percentage
    double value;
    string unit;
    if (consumePercentage(value))
    {
      func.values.push_back(value / 100.0); // Convert percentage to decimal
      func.units.push_back("%");
    }
    else if (consumeNumber(value, unit))
    {
      func.values.push_back(value);
      func.units.push_back(unit);
    }
    else
    {
      setError("Expected number or percentage for grayscale()");
      return nullopt;
    }

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis for grayscale()");
      return nullopt;
    }

    return func;
  }

  optional<FilterFunction> CSSFilterParser::parseHueRotate()
  {
    FilterFunction func(FilterFunctionType::kHueRotate);

    // hue-rotate() with no arguments defaults to 0deg
    if (!isAtEnd() && currentToken().type == TokenType::kRightParen)
    {
      advance(); // Consume closing paren
      func.values.push_back(0.0);
      func.units.push_back("deg");
      return func;
    }

    // Parse angle value
    double value;
    string unit;
    if (!consumeAngle(value, unit))
    {
      setError("Expected angle value for hue-rotate()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis for hue-rotate()");
      return nullopt;
    }

    return func;
  }

  optional<FilterFunction> CSSFilterParser::parseInvert()
  {
    FilterFunction func(FilterFunctionType::kInvert);

    // invert() with no arguments defaults to 1
    if (!isAtEnd() && currentToken().type == TokenType::kRightParen)
    {
      advance(); // Consume closing paren
      func.values.push_back(1.0);
      func.units.push_back("");
      return func;
    }

    // Parse number or percentage
    double value;
    string unit;
    if (consumePercentage(value))
    {
      func.values.push_back(value / 100.0); // Convert percentage to decimal
      func.units.push_back("%");
    }
    else if (consumeNumber(value, unit))
    {
      func.values.push_back(value);
      func.units.push_back(unit);
    }
    else
    {
      setError("Expected number or percentage for invert()");
      return nullopt;
    }

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis for invert()");
      return nullopt;
    }

    return func;
  }

  optional<FilterFunction> CSSFilterParser::parseOpacity()
  {
    FilterFunction func(FilterFunctionType::kOpacity);

    // opacity() with no arguments defaults to 1
    if (!isAtEnd() && currentToken().type == TokenType::kRightParen)
    {
      advance(); // Consume closing paren
      func.values.push_back(1.0);
      func.units.push_back("");
      return func;
    }

    // Parse number or percentage
    double value;
    string unit;
    if (consumePercentage(value))
    {
      func.values.push_back(value / 100.0); // Convert percentage to decimal
      func.units.push_back("%");
    }
    else if (consumeNumber(value, unit))
    {
      func.values.push_back(value);
      func.units.push_back(unit);
    }
    else
    {
      setError("Expected number or percentage for opacity()");
      return nullopt;
    }

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis for opacity()");
      return nullopt;
    }

    return func;
  }

  optional<FilterFunction> CSSFilterParser::parseSaturate()
  {
    FilterFunction func(FilterFunctionType::kSaturate);

    // saturate() with no arguments defaults to 1
    if (!isAtEnd() && currentToken().type == TokenType::kRightParen)
    {
      advance(); // Consume closing paren
      func.values.push_back(1.0);
      func.units.push_back("");
      return func;
    }

    // Parse number or percentage
    double value;
    string unit;
    if (consumePercentage(value))
    {
      func.values.push_back(value / 100.0); // Convert percentage to decimal
      func.units.push_back("%");
    }
    else if (consumeNumber(value, unit))
    {
      func.values.push_back(value);
      func.units.push_back(unit);
    }
    else
    {
      setError("Expected number or percentage for saturate()");
      return nullopt;
    }

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis for saturate()");
      return nullopt;
    }

    return func;
  }

  optional<FilterFunction> CSSFilterParser::parseSepia()
  {
    FilterFunction func(FilterFunctionType::kSepia);

    // sepia() with no arguments defaults to 1
    if (!isAtEnd() && currentToken().type == TokenType::kRightParen)
    {
      advance(); // Consume closing paren
      func.values.push_back(1.0);
      func.units.push_back("");
      return func;
    }

    // Parse number or percentage
    double value;
    string unit;
    if (consumePercentage(value))
    {
      func.values.push_back(value / 100.0); // Convert percentage to decimal
      func.units.push_back("%");
    }
    else if (consumeNumber(value, unit))
    {
      func.values.push_back(value);
      func.units.push_back(unit);
    }
    else
    {
      setError("Expected number or percentage for sepia()");
      return nullopt;
    }

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis for sepia()");
      return nullopt;
    }

    return func;
  }

  bool CSSFilterParser::consumeToken(TokenType expected_type)
  {
    if (isAtEnd() || currentToken().type != expected_type)
    {
      return false;
    }
    advance();
    return true;
  }

  bool CSSFilterParser::consumeComma()
  {
    return consumeToken(TokenType::kComma);
  }

  bool CSSFilterParser::consumeNumber(double &value, string &unit)
  {
    if (isAtEnd())
      return false;

    const auto &token = currentToken();
    if (token.type == TokenType::kNumber)
    {
      value = token.numeric_value;
      unit = "";
      advance();
      return true;
    }
    else if (token.type == TokenType::kDimension)
    {
      value = token.numeric_value;
      unit = token.unit;
      advance();
      return true;
    }

    return false;
  }

  bool CSSFilterParser::consumeLength(double &value, string &unit)
  {
    if (!consumeNumber(value, unit))
      return false;

    // Validate length units
    if (unit.empty() && value == 0.0)
    {
      unit = "px"; // Zero length can be unitless
      return true;
    }

    if (unit == "px" || unit == "em" || unit == "rem" || unit == "vh" || unit == "vw" ||
        unit == "pt" || unit == "pc" || unit == "in" || unit == "cm" || unit == "mm")
    {
      return true;
    }

    setError("Invalid length unit: " + unit);
    return false;
  }

  bool CSSFilterParser::consumeAngle(double &value, string &unit)
  {
    if (!consumeNumber(value, unit))
      return false;

    // Validate angle units
    if (unit.empty() && value == 0.0)
    {
      unit = "deg"; // Zero angle can be unitless
      return true;
    }

    if (unit == "deg" || unit == "rad" || unit == "grad" || unit == "turn")
    {
      return true;
    }

    setError("Invalid angle unit: " + unit);
    return false;
  }

  bool CSSFilterParser::consumePercentage(double &value)
  {
    if (isAtEnd() || currentToken().type != TokenType::kPercentage)
      return false;

    value = currentToken().numeric_value;
    advance();
    return true;
  }

  bool CSSFilterParser::isAtEnd() const
  {
    return current_token_index_ >= tokens_.size();
  }

  const Token &CSSFilterParser::currentToken() const
  {
    static Token dummy_token(TokenType::kWhitespace);
    if (isAtEnd())
      return dummy_token;
    return tokens_[current_token_index_];
  }

  void CSSFilterParser::advance()
  {
    if (current_token_index_ < tokens_.size())
    {
      current_token_index_++;
    }
  }

  void CSSFilterParser::setError(const string &message)
  {
    is_valid_ = false;
    error_message_ = message;
  }

  FilterFunctionType CSSFilterParser::getFunctionType(const string &name)
  {
    static const unordered_map<string, FilterFunctionType> function_map = {
      {"blur", FilterFunctionType::kBlur},
      {"brightness", FilterFunctionType::kBrightness},
      {"contrast", FilterFunctionType::kContrast},
      {"drop-shadow", FilterFunctionType::kDropShadow},
      {"grayscale", FilterFunctionType::kGrayscale},
      {"hue-rotate", FilterFunctionType::kHueRotate},
      {"invert", FilterFunctionType::kInvert},
      {"opacity", FilterFunctionType::kOpacity},
      {"saturate", FilterFunctionType::kSaturate},
      {"sepia", FilterFunctionType::kSepia}};

    auto it = function_map.find(name);
    return (it != function_map.end()) ? it->second : FilterFunctionType::kNone;
  }

  void CSSFilterParser::skipSeparators()
  {
    while (!isAtEnd())
    {
      const auto &token = currentToken();
      if (token.type == TokenType::kWhitespace || token.type == TokenType::kComma)
      {
        advance();
      }
      else
      {
        break;
      }
    }
  }
}