#include <cctype>
#include <algorithm>

#include "./css_value_tokenizer.hpp"

namespace client_cssom::css_value_tokenizer
{
  using namespace std;

  CSSValueTokenizer::CSSValueTokenizer(const string &input)
      : input_(input)
      , position_(0)
      , length_(input.length())
  {
  }

  vector<Token> CSSValueTokenizer::tokenize()
  {
    vector<Token> tokens;
    reset();

    while (hasNext())
    {
      Token token = nextToken();
      if (token.type != TokenType::kWhitespace)
      {
        tokens.push_back(token);
      }
    }

    return tokens;
  }

  Token CSSValueTokenizer::nextToken()
  {
    skip_whitespace();

    if (position_ >= length_)
    {
      return Token(TokenType::kWhitespace); // Return dummy token when done
    }

    char c = current_char();
    size_t token_start = position_;

    // String literals
    if (c == '"' || c == '\'')
    {
      auto token = consume_string(c);
      token.start_position = token_start;
      token.end_position = position_;
      return token;
    }

    // Numbers
    if (is_digit(c) || (c == '.' && is_digit(peek_char())))
    {
      auto token = consume_number();
      token.start_position = token_start;
      token.end_position = position_;
      return token;
    }

    // Identifiers and functions
    if (is_identifier_start(c))
    {
      string identifier = consume_identifier_sequence();

      // Check if it's a function
      if (position_ < length_ && current_char() == '(')
      {
        if (identifier == "url" || identifier == "src")
        {
          auto token = consume_url();
          token.start_position = token_start;
          token.end_position = position_;
          return token;
        }
        else
        {
          auto token = consume_function(identifier);
          token.start_position = token_start;
          token.end_position = position_;
          return token;
        }
      }

      return Token(TokenType::kIdentifier, identifier);
    }

    // Single character tokens
    switch (c)
    {
    case '(':
      advance();
      return Token(TokenType::kLeftParen, "(");
    case ')':
      advance();
      return Token(TokenType::kRightParen, ")");
    case ',':
      advance();
      return Token(TokenType::kComma, ",");
    case '#':
    {
      auto token = consume_hash();
      token.start_position = token_start;
      token.end_position = position_;
      return token;
    }
    default:
      advance();
      return Token(TokenType::kDelimiter, string(1, c));
    }
  }

  bool CSSValueTokenizer::hasNext() const
  {
    return position_ < length_;
  }

  void CSSValueTokenizer::reset()
  {
    position_ = 0;
  }

  char CSSValueTokenizer::current_char() const
  {
    return position_ < length_ ? input_[position_] : '\0';
  }

  char CSSValueTokenizer::peek_char(size_t offset) const
  {
    size_t peek_pos = position_ + offset;
    return peek_pos < length_ ? input_[peek_pos] : '\0';
  }

  void CSSValueTokenizer::advance()
  {
    if (position_ < length_)
    {
      position_++;
    }
  }

  void CSSValueTokenizer::skip_whitespace()
  {
    while (position_ < length_ && is_whitespace(current_char()))
    {
      advance();
    }
  }

  bool CSSValueTokenizer::is_whitespace(char c) const
  {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f';
  }

  bool CSSValueTokenizer::is_letter(char c) const
  {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  }

  bool CSSValueTokenizer::is_digit(char c) const
  {
    return c >= '0' && c <= '9';
  }

  bool CSSValueTokenizer::is_hex_digit(char c) const
  {
    return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
  }

  bool CSSValueTokenizer::is_identifier_start(char c) const
  {
    return is_letter(c) || c == '_' || c == '-' || static_cast<unsigned char>(c) >= 0x80;
  }

  bool CSSValueTokenizer::is_identifier_char(char c) const
  {
    return is_identifier_start(c) || is_digit(c);
  }

  Token CSSValueTokenizer::consume_identifier()
  {
    return Token(TokenType::kIdentifier, consume_identifier_sequence());
  }

  Token CSSValueTokenizer::consume_string(char quote_char)
  {
    advance(); // Skip opening quote
    string value;

    while (position_ < length_ && current_char() != quote_char)
    {
      char c = current_char();
      if (c == '\\')
      {
        consume_escape_sequence(value);
      }
      else if (c == '\n' || c == '\r' || c == '\f')
      {
        // Bad string
        return Token(TokenType::kBadString, value);
      }
      else
      {
        value += c;
        advance();
      }
    }

    if (position_ < length_ && current_char() == quote_char)
    {
      advance(); // Skip closing quote
    }

    return Token(TokenType::kString, value);
  }

  Token CSSValueTokenizer::consume_number()
  {
    string number_str;

    // Consume integer part
    while (position_ < length_ && is_digit(current_char()))
    {
      number_str += current_char();
      advance();
    }

    // Consume decimal part
    if (position_ < length_ && current_char() == '.' &&
        position_ + 1 < length_ && is_digit(peek_char()))
    {
      number_str += current_char();
      advance();
      while (position_ < length_ && is_digit(current_char()))
      {
        number_str += current_char();
        advance();
      }
    }

    // Convert to number
    double numeric_value = stod(number_str);

    // Check for percentage
    if (position_ < length_ && current_char() == '%')
    {
      advance();
      return Token(TokenType::kPercentage, number_str + "%", numeric_value);
    }

    // Check for dimension (unit)
    if (position_ < length_ && is_identifier_start(current_char()))
    {
      string unit = consume_identifier_sequence();
      return Token(TokenType::kDimension, number_str + unit, unit, numeric_value);
    }

    return Token(TokenType::kNumber, number_str, numeric_value);
  }

  Token CSSValueTokenizer::consume_url()
  {
    advance(); // Skip '('
    skip_whitespace();

    string url_value;

    // Check if it starts with a quote
    if (position_ < length_ && (current_char() == '"' || current_char() == '\''))
    {
      char quote_char = current_char();
      Token string_token = consume_string(quote_char);
      if (string_token.type == TokenType::kBadString)
      {
        return Token(TokenType::kBadUrl, url_value);
      }
      url_value = string_token.value;
    }
    else
    {
      // Unquoted URL
      while (position_ < length_ && current_char() != ')' && !is_whitespace(current_char()))
      {
        char c = current_char();
        if (c == '\\')
        {
          consume_escape_sequence(url_value);
        }
        else if (c == '"' || c == '\'' || c == '(' || static_cast<unsigned char>(c) < 0x20)
        {
          // Bad URL
          return Token(TokenType::kBadUrl, url_value);
        }
        else
        {
          url_value += c;
          advance();
        }
      }
    }

    skip_whitespace();

    if (position_ < length_ && current_char() == ')')
    {
      advance(); // Skip ')'
      return Token(TokenType::kUrl, url_value);
    }

    return Token(TokenType::kBadUrl, url_value);
  }

  Token CSSValueTokenizer::consume_function(const string &name)
  {
    advance(); // Skip '('
    return Token(TokenType::kFunction, name);
  }

  Token CSSValueTokenizer::consume_hash()
  {
    advance(); // Skip '#'
    string hash_value;

    // Consume hex digits for hex color format
    while (position_ < length_ && is_hex_digit(current_char()))
    {
      hash_value += current_char();
      advance();
    }

    // If no hex digits found or invalid length, treat as identifier
    if (hash_value.empty() || (hash_value.length() != 3 && hash_value.length() != 4 &&
                               hash_value.length() != 6 && hash_value.length() != 8))
    {
      // Continue consuming as identifier characters
      while (position_ < length_ && is_identifier_char(current_char()))
      {
        hash_value += current_char();
        advance();
      }
    }

    return Token(TokenType::kHash, hash_value);
  }

  string CSSValueTokenizer::consume_identifier_sequence()
  {
    string identifier;

    while (position_ < length_ && is_identifier_char(current_char()))
    {
      char c = current_char();
      if (c == '\\')
      {
        consume_escape_sequence(identifier);
      }
      else
      {
        identifier += c;
        advance();
      }
    }

    return identifier;
  }

  void CSSValueTokenizer::consume_escape_sequence(string &result)
  {
    advance(); // Skip '\'

    if (position_ >= length_)
    {
      return;
    }

    char c = current_char();
    if (is_hex_digit(c))
    {
      // Hex escape sequence
      string hex_digits;
      for (int i = 0; i < 6 && position_ < length_ && is_hex_digit(current_char()); i++)
      {
        hex_digits += current_char();
        advance();
      }

      // Skip optional whitespace after hex digits
      if (position_ < length_ && is_whitespace(current_char()))
      {
        advance();
      }

      // Convert hex to character
      if (!hex_digits.empty())
      {
        int codepoint = stoi(hex_digits, nullptr, 16);
        if (codepoint == 0 || codepoint > 0x10FFFF)
        {
          result += "�"; // Replacement character
        }
        else if (codepoint < 0x80)
        {
          // Simple ASCII handling
          result += static_cast<char>(codepoint);
        }
        else
        {
          // For non-ASCII, just add the replacement character for now
          result += "�";
        }
      }
    }
    else if (c == '\n' || c == '\r' || c == '\f')
    {
      // Invalid escape sequence
      return;
    }
    else
    {
      // Any other character
      result += c;
      advance();
    }
  }
}
