#include "css_variable_parser.hpp"
#include <algorithm>
#include <cctype>

namespace client_cssom::css_variable_parser
{
  using namespace std;

  CSSVariableParser::CSSVariableParser(const string &input)
      : input_(input)
      , is_valid_(true)
  {
  }

  string CSSVariableParser::resolveVariables(const VariableResolver &resolver)
  {
    if (input_.empty())
    {
      return input_;
    }

    return resolveVariablesRecursive(input_, resolver, 0);
  }

  string CSSVariableParser::resolveVariablesRecursive(const string &value, const VariableResolver &resolver, int depth)
  {
    // Prevent infinite recursion
    if (depth > MAX_RECURSION_DEPTH)
    {
      is_valid_ = false;
      error_message_ = "Maximum recursion depth exceeded while resolving CSS variables";
      return value;
    }

    string result = value;

    // Keep processing until no more variables are found
    bool found_variable = true;
    while (found_variable)
    {
      found_variable = false;
      size_t pos = 0;

      // Find the first var() in the current result string
      while (pos < result.length())
      {
        size_t var_pos = result.find("var(", pos);
        if (var_pos == string::npos)
        {
          break; // No more variables found
        }

        // Parse this variable
        CSSVariableParser temp_parser(result.substr(var_pos));
        auto var_ref = temp_parser.findNextVariable(0);
        if (!var_ref.has_value())
        {
          pos = var_pos + 4; // Skip this "var(" and continue
          continue;
        }

        const auto &var = var_ref.value();

        // Adjust positions to be relative to the result string
        size_t actual_start = var_pos + var.start_position;
        size_t actual_end = var_pos + var.end_position;

        // Try to resolve the variable
        auto resolved_value = resolver(var.variable_name);

        string replacement;
        if (resolved_value.has_value())
        {
          // Variable found, use its value (don't recurse yet, we'll handle it in the next iteration)
          replacement = resolved_value.value();
        }
        else if (var.fallback_value.has_value())
        {
          // Variable not found, use fallback (don't recurse yet, we'll handle it in the next iteration)
          replacement = var.fallback_value.value();
        }
        else
        {
          // No variable found and no fallback, keep the original var() function
          pos = actual_end + 1;
          continue;
        }

        // Replace the var() function with the resolved value
        result.replace(actual_start, actual_end - actual_start + 1, replacement);

        // Mark that we found and replaced a variable
        found_variable = true;
        break; // Start over from the beginning after each replacement
      }
    }

    return result;
  }

  vector<VariableReference> CSSVariableParser::getVariableReferences()
  {
    vector<VariableReference> references;
    size_t pos = 0;

    while (pos < input_.length())
    {
      auto var_ref = findNextVariable(pos);
      if (!var_ref.has_value())
      {
        break;
      }

      references.push_back(var_ref.value());
      pos = var_ref.value().end_position + 1;
    }

    return references;
  }

  optional<VariableReference> CSSVariableParser::findNextVariable(size_t start_pos)
  {
    // Look for "var(" pattern
    size_t var_pos = input_.find("var(", start_pos);
    if (var_pos == string::npos)
    {
      return nullopt;
    }

    return parseVariableContent(var_pos);
  }

  optional<VariableReference> CSSVariableParser::parseVariableContent(size_t start_pos)
  {
    // start_pos should point to the 'v' in "var("
    if (start_pos + 4 >= input_.length() || input_.substr(start_pos, 4) != "var(")
    {
      return nullopt;
    }

    size_t content_start = start_pos + 4;                    // Skip "var("
    size_t paren_end = findMatchingParen(content_start - 1); // -1 to point to the opening paren

    if (paren_end == string::npos)
    {
      is_valid_ = false;
      error_message_ = "Unclosed var() function";
      return nullopt;
    }

    // Extract content between parentheses
    string content = input_.substr(content_start, paren_end - content_start);

    // Parse variable name and fallback
    auto [var_name, fallback] = parseVariableNameAndFallback(content);

    if (var_name.empty())
    {
      is_valid_ = false;
      error_message_ = "Empty variable name in var() function";
      return nullopt;
    }

    if (fallback.has_value())
    {
      return VariableReference(var_name, fallback.value(), start_pos, paren_end);
    }
    else
    {
      return VariableReference(var_name, start_pos, paren_end);
    }
  }

  size_t CSSVariableParser::findMatchingParen(size_t start_pos)
  {
    if (start_pos >= input_.length() || input_[start_pos] != '(')
    {
      return string::npos;
    }

    int paren_count = 1;
    size_t pos = start_pos + 1;

    while (pos < input_.length() && paren_count > 0)
    {
      char c = input_[pos];
      if (c == '(')
      {
        paren_count++;
      }
      else if (c == ')')
      {
        paren_count--;
      }
      else if (c == '"' || c == '\'')
      {
        // Skip quoted strings to avoid counting parentheses inside them
        char quote = c;
        pos++;
        while (pos < input_.length() && input_[pos] != quote)
        {
          if (input_[pos] == '\\')
          {
            pos++; // Skip escaped character
          }
          pos++;
        }
      }
      pos++;
    }

    return (paren_count == 0) ? pos - 1 : string::npos;
  }

  pair<string, optional<string>> CSSVariableParser::parseVariableNameAndFallback(const string &content)
  {
    // Find the first comma that's not inside parentheses or quotes
    size_t comma_pos = string::npos;
    int paren_count = 0;
    bool in_quotes = false;
    char quote_char = '\0';

    for (size_t i = 0; i < content.length(); i++)
    {
      char c = content[i];

      if (!in_quotes)
      {
        if (c == '"' || c == '\'')
        {
          in_quotes = true;
          quote_char = c;
        }
        else if (c == '(')
        {
          paren_count++;
        }
        else if (c == ')')
        {
          paren_count--;
        }
        else if (c == ',' && paren_count == 0)
        {
          comma_pos = i;
          break;
        }
      }
      else
      {
        if (c == quote_char)
        {
          in_quotes = false;
        }
        else if (c == '\\')
        {
          i++; // Skip escaped character
        }
      }
    }

    if (comma_pos == string::npos)
    {
      // No fallback
      return {trim(content), nullopt};
    }
    else
    {
      // Has fallback
      string var_name = trim(content.substr(0, comma_pos));
      string fallback = trim(content.substr(comma_pos + 1));
      return {var_name, fallback};
    }
  }

  string CSSVariableParser::trim(const string &str)
  {
    auto start = str.find_first_not_of(" \t\n\r\f");
    if (start == string::npos)
    {
      return "";
    }
    auto end = str.find_last_not_of(" \t\n\r\f");
    return str.substr(start, end - start + 1);
  }
}
