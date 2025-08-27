#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include "./css_value_tokenizer.hpp"

namespace client_cssom::css_filter_parser
{
  // Enum for different filter function types
  enum class FilterFunctionType
  {
    kNone,
    kBlur,
    kBrightness,
    kContrast,
    kDropShadow,
    kGrayscale,
    kHueRotate,
    kInvert,
    kOpacity,
    kSaturate,
    kSepia
  };

  // Structure to hold parsed filter function data
  struct FilterFunction
  {
    FilterFunctionType type;
    std::vector<double> values;     // Numeric values
    std::vector<std::string> units; // Units for each value
    std::string raw_value;          // Original string for complex values like drop-shadow

    FilterFunction(FilterFunctionType t)
        : type(t)
    {
    }
  };

  // Main parser class
  class CSSFilterParser
  {
  public:
    explicit CSSFilterParser(const std::string &input);

    // Parse the filter string and return list of functions
    std::vector<FilterFunction> parse();

    // Check if parsing was successful
    bool isValid() const
    {
      return is_valid_;
    }

    // Get error message if parsing failed
    const std::string &getError() const
    {
      return error_message_;
    }

  private:
    std::string input_;
    css_value_tokenizer::CSSValueTokenizer tokenizer_;
    std::vector<css_value_tokenizer::Token> tokens_;
    size_t current_token_index_;
    bool is_valid_;
    std::string error_message_;

    // Parse individual filter functions
    std::optional<FilterFunction> parseFilterFunction();
    std::optional<FilterFunction> parseBlur();
    std::optional<FilterFunction> parseBrightness();
    std::optional<FilterFunction> parseContrast();
    std::optional<FilterFunction> parseDropShadow();
    std::optional<FilterFunction> parseGrayscale();
    std::optional<FilterFunction> parseHueRotate();
    std::optional<FilterFunction> parseInvert();
    std::optional<FilterFunction> parseOpacity();
    std::optional<FilterFunction> parseSaturate();
    std::optional<FilterFunction> parseSepia();

    // Helper methods
    bool consumeToken(css_value_tokenizer::TokenType expected_type);
    bool consumeComma();
    bool consumeNumber(double &value, std::string &unit);
    bool consumeLength(double &value, std::string &unit);
    bool consumeAngle(double &value, std::string &unit);
    bool consumePercentage(double &value);
    bool isAtEnd() const;
    const css_value_tokenizer::Token &currentToken() const;
    void advance();
    void setError(const std::string &message);

    // Filter function name to type mapping
    static FilterFunctionType getFunctionType(const std::string &name);

    // Skip whitespace and commas between functions
    void skipSeparators();
  };
}