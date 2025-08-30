#pragma once

#include <string>
#include <vector>
#include <optional>
#include <functional>
#include "./css_value_tokenizer.hpp"

namespace client_cssom::css_variable_parser
{
  // Structure to hold parsed variable information
  struct VariableReference
  {
    std::string variable_name;
    std::optional<std::string> fallback_value;
    size_t start_position;
    size_t end_position;

    VariableReference(const std::string &name, size_t start, size_t end)
        : variable_name(name)
        , start_position(start)
        , end_position(end)
    {
    }

    VariableReference(const std::string &name, const std::string &fallback, size_t start, size_t end)
        : variable_name(name)
        , fallback_value(fallback)
        , start_position(start)
        , end_position(end)
    {
    }
  };

  // Variable resolver function type
  // Takes variable name and returns resolved value, or empty optional if not found
  using VariableResolver = std::function<std::optional<std::string>(const std::string &)>;

  // Main parser class
  class CSSVariableParser
  {
  public:
    explicit CSSVariableParser(const std::string &input);

    // Parse and resolve all CSS variables in the input string
    std::string resolveVariables(const VariableResolver &resolver);

    // Get all variable references found in the input (without resolving)
    std::vector<VariableReference> getVariableReferences();

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
    bool is_valid_;
    std::string error_message_;

    // Find the next var() function in the string starting from the given position
    std::optional<VariableReference> findNextVariable(size_t start_pos);

    // Parse the content inside var() parentheses
    std::optional<VariableReference> parseVariableContent(size_t start_pos);

    // Find matching closing parenthesis, handling nested parentheses
    size_t findMatchingParen(size_t start_pos);

    // Parse variable name and optional fallback from var() content
    std::pair<std::string, std::optional<std::string>> parseVariableNameAndFallback(const std::string &content);

    // Trim whitespace from string
    std::string trim(const std::string &str);

    // Recursively resolve variables, handling nested var() calls
    std::string resolveVariablesRecursive(const std::string &value, const VariableResolver &resolver, int depth = 0);

    // Maximum recursion depth to prevent infinite loops
    static const int MAX_RECURSION_DEPTH = 10;
  };
}
