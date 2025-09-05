#include <cctype>
#include <algorithm>
#include <sstream>

#include "./css_tokenizer.hpp"

namespace client_cssom::css_tokenizer
{
  using namespace std;

  CSSTokenizer::CSSTokenizer(const string &input)
      : input_(input)
      , position_(0)
      , length_(input.length())
  {
  }

  vector<Token> CSSTokenizer::tokenize()
  {
    vector<Token> tokens;
    reset();

    while (hasNext())
    {
      Token token = nextToken();
      if (token.type != TokenType::kComment) // Skip comments but keep other whitespace
      {
        tokens.push_back(token);
      }
    }

    // Add EOF token
    tokens.push_back(Token(TokenType::kEOF));
    return tokens;
  }

  Token CSSTokenizer::nextToken()
  {
    if (position_ >= length_)
    {
      return Token(TokenType::kEOF);
    }

    char c = current_char();
    size_t token_start = position_;

    // Whitespace
    if (is_whitespace(c))
    {
      skip_whitespace();
      auto token = Token(TokenType::kWhitespace, " ");
      token.start_position = token_start;
      token.end_position = position_;
      return token;
    }

    // Comments
    if (c == '/' && peek_char() == '*')
    {
      auto token = consume_comment();
      token.start_position = token_start;
      token.end_position = position_;
      return token;
    }

    // String literals
    if (c == '"' || c == '\'')
    {
      auto token = consume_string(c);
      token.start_position = token_start;
      token.end_position = position_;
      return token;
    }

    // Numbers
    if (would_start_number())
    {
      auto token = consume_numeric();
      token.start_position = token_start;
      token.end_position = position_;
      return token;
    }

    // Hash
    if (c == '#')
    {
      auto token = consume_hash();
      token.start_position = token_start;
      token.end_position = position_;
      return token;
    }

    // At-keyword
    if (c == '@')
    {
      if (position_ + 1 < length_ && is_identifier_start(peek_char()))
      {
        auto token = consume_at_keyword();
        token.start_position = token_start;
        token.end_position = position_;
        return token;
      }
      else
      {
        advance();
        return Token(TokenType::kDelimiter, "@");
      }
    }

    // Identifiers and functions
    if (would_start_identifier())
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

    // Multi-character delimiters
    if (c == '<' && peek_char() == '!' && peek_char(2) == '-' && peek_char(3) == '-')
    {
      advance();
      advance();
      advance();
      advance();
      return Token(TokenType::kCDO, "<!--");
    }

    if (c == '-' && peek_char() == '-' && peek_char(2) == '>')
    {
      advance();
      advance();
      advance();
      return Token(TokenType::kCDC, "-->");
    }

    if (c == '|' && peek_char() == '|')
    {
      advance();
      advance();
      return Token(TokenType::kColumn, "||");
    }

    if (c == '~' && peek_char() == '=')
    {
      advance();
      advance();
      return Token(TokenType::kIncludeMatch, "~=");
    }

    if (c == '|' && peek_char() == '=')
    {
      advance();
      advance();
      return Token(TokenType::kDashMatch, "|=");
    }

    if (c == '^' && peek_char() == '=')
    {
      advance();
      advance();
      return Token(TokenType::kPrefixMatch, "^=");
    }

    if (c == '$' && peek_char() == '=')
    {
      advance();
      advance();
      return Token(TokenType::kSuffixMatch, "$=");
    }

    if (c == '*' && peek_char() == '=')
    {
      advance();
      advance();
      return Token(TokenType::kSubstringMatch, "*=");
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
    case '[':
      advance();
      return Token(TokenType::kLeftSquareBracket, "[");
    case ']':
      advance();
      return Token(TokenType::kRightSquareBracket, "]");
    case '{':
      advance();
      return Token(TokenType::kLeftCurlyBracket, "{");
    case '}':
      advance();
      return Token(TokenType::kRightCurlyBracket, "}");
    case ',':
      advance();
      return Token(TokenType::kComma, ",");
    case ':':
      advance();
      return Token(TokenType::kColon, ":");
    case ';':
      advance();
      return Token(TokenType::kSemicolon, ";");
    default:
      advance();
      return Token(TokenType::kDelimiter, string(1, c));
    }
  }

  bool CSSTokenizer::hasNext() const
  {
    return position_ < length_;
  }

  void CSSTokenizer::reset()
  {
    position_ = 0;
  }

  char CSSTokenizer::current_char() const
  {
    return position_ < length_ ? input_[position_] : '\0';
  }

  char CSSTokenizer::peek_char(size_t offset) const
  {
    size_t peek_pos = position_ + offset;
    return peek_pos < length_ ? input_[peek_pos] : '\0';
  }

  void CSSTokenizer::advance()
  {
    if (position_ < length_)
    {
      position_++;
    }
  }

  void CSSTokenizer::skip_whitespace()
  {
    while (position_ < length_ && is_whitespace(current_char()))
    {
      advance();
    }
  }

  bool CSSTokenizer::is_whitespace(char c) const
  {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f';
  }

  bool CSSTokenizer::is_letter(char c) const
  {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
  }

  bool CSSTokenizer::is_digit(char c) const
  {
    return c >= '0' && c <= '9';
  }

  bool CSSTokenizer::is_hex_digit(char c) const
  {
    return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
  }

  bool CSSTokenizer::is_identifier_start(char c) const
  {
    return is_letter(c) || c == '_' || static_cast<unsigned char>(c) >= 0x80;
  }

  bool CSSTokenizer::is_identifier_char(char c) const
  {
    return is_identifier_start(c) || is_digit(c) || c == '-';
  }

  bool CSSTokenizer::is_non_printable(char c) const
  {
    return (c >= '\0' && c <= '\x08') || c == '\x0B' || (c >= '\x0E' && c <= '\x1F') || c == '\x7F';
  }

  Token CSSTokenizer::consume_identifier()
  {
    return Token(TokenType::kIdentifier, consume_identifier_sequence());
  }

  Token CSSTokenizer::consume_string(char quote_char)
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
    else
    {
      return Token(TokenType::kBadString, value);
    }

    return Token(TokenType::kString, value);
  }

  Token CSSTokenizer::consume_numeric()
  {
    string number_str;
    bool has_sign = false;

    // Handle sign
    if (current_char() == '+' || current_char() == '-')
    {
      number_str += current_char();
      advance();
      has_sign = true;
    }

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

    // Handle exponent
    if (position_ < length_ && (current_char() == 'e' || current_char() == 'E'))
    {
      size_t saved_pos = position_;
      number_str += current_char();
      advance();

      if (position_ < length_ && (current_char() == '+' || current_char() == '-'))
      {
        number_str += current_char();
        advance();
      }

      if (position_ < length_ && is_digit(current_char()))
      {
        while (position_ < length_ && is_digit(current_char()))
        {
          number_str += current_char();
          advance();
        }
      }
      else
      {
        // Invalid exponent, backtrack
        position_ = saved_pos;
        number_str.erase(saved_pos - (has_sign ? 1 : 0));
      }
    }

    // Convert to number
    double numeric_value = 0.0;
    try
    {
      numeric_value = stod(number_str);
    }
    catch (...)
    {
      numeric_value = 0.0;
    }

    // Check for percentage
    if (position_ < length_ && current_char() == '%')
    {
      advance();
      return Token(TokenType::kPercentage, number_str + "%", numeric_value);
    }

    // Check for dimension (unit)
    if (would_start_identifier())
    {
      string unit = consume_identifier_sequence();
      return Token(TokenType::kDimension, number_str + unit, unit, numeric_value);
    }

    return Token(TokenType::kNumber, number_str, numeric_value);
  }

  Token CSSTokenizer::consume_url()
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
        else if (c == '"' || c == '\'' || c == '(' || is_non_printable(c))
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

  Token CSSTokenizer::consume_function(const string &name)
  {
    advance(); // Skip '('
    return Token(TokenType::kFunction, name);
  }

  Token CSSTokenizer::consume_at_keyword()
  {
    advance(); // Skip '@'
    string identifier = consume_identifier_sequence();
    return Token(TokenType::kAtKeyword, "@" + identifier);
  }

  Token CSSTokenizer::consume_hash()
  {
    advance(); // Skip '#'
    string hash_value;

    // Hash token can contain identifier characters or hex digits
    while (position_ < length_ &&
           (is_identifier_char(current_char()) || is_hex_digit(current_char())))
    {
      if (current_char() == '\\')
      {
        consume_escape_sequence(hash_value);
      }
      else
      {
        hash_value += current_char();
        advance();
      }
    }

    if (hash_value.empty())
    {
      return Token(TokenType::kDelimiter, "#");
    }

    return Token(TokenType::kHash, "#" + hash_value);
  }

  Token CSSTokenizer::consume_comment()
  {
    advance(); // Skip '/'
    advance(); // Skip '*'

    string comment_value;

    while (position_ < length_)
    {
      if (current_char() == '*' && peek_char() == '/')
      {
        advance(); // Skip '*'
        advance(); // Skip '/'
        break;
      }
      comment_value += current_char();
      advance();
    }

    return Token(TokenType::kComment, comment_value);
  }

  string CSSTokenizer::consume_identifier_sequence()
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

  void CSSTokenizer::consume_escape_sequence(string &result)
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
          result += "\uFFFD"; // Replacement character
        }
        else if (codepoint < 0x80)
        {
          // Simple ASCII handling
          result += static_cast<char>(codepoint);
        }
        else
        {
          // For non-ASCII, just add the replacement character for now
          result += "\uFFFD";
        }
      }
    }
    else if (c == '\n' || c == '\r' || c == '\f')
    {
      // Invalid escape sequence, ignore
      return;
    }
    else
    {
      // Any other character
      result += c;
      advance();
    }
  }

  bool CSSTokenizer::would_start_identifier() const
  {
    if (position_ >= length_)
      return false;

    char c = current_char();
    if (is_identifier_start(c))
      return true;

    if (c == '-')
    {
      if (position_ + 1 < length_)
      {
        char next = peek_char();
        return is_identifier_start(next) || next == '-' || next == '\\';
      }
    }

    if (c == '\\')
    {
      return position_ + 1 < length_; // Valid escape sequence
    }

    return false;
  }

  bool CSSTokenizer::would_start_number() const
  {
    if (position_ >= length_)
      return false;

    char c = current_char();
    if (is_digit(c))
      return true;

    if (c == '.')
    {
      return position_ + 1 < length_ && is_digit(peek_char());
    }

    if (c == '+' || c == '-')
    {
      if (position_ + 1 < length_)
      {
        char next = peek_char();
        if (is_digit(next))
          return true;
        if (next == '.' && position_ + 2 < length_ && is_digit(peek_char(2)))
          return true;
      }
    }

    return false;
  }
}