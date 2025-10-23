#pragma once

#include <string>
#include <vector>
#include <variant>

namespace client_cssom::css_value_tokenizer
{
  enum class TokenType
  {
    kIdentifier,
    kFunction,
    kString,
    kUrl,
    kNumber,
    kPercentage,
    kDimension,
    kHash,
    kDelimiter,
    kComma,
    kLeftParen,
    kRightParen,
    kWhitespace,
    kBadString,
    kBadUrl
  };

  struct Token
  {
    TokenType type;
    std::string value;
    std::string unit;           // For dimension tokens
    double numeric_value = 0.0; // For number/percentage/dimension tokens
    size_t start_position = 0;
    size_t end_position = 0;

    Token(TokenType t, const std::string &v = "")
        : type(t)
        , value(v)
    {
    }
    Token(TokenType t, const std::string &v, double num)
        : type(t)
        , value(v)
        , numeric_value(num)
    {
    }
    Token(TokenType t, const std::string &v, const std::string &u, double num)
        : type(t)
        , value(v)
        , unit(u)
        , numeric_value(num)
    {
    }
  };

  class CSSValueTokenizer
  {
  public:
    explicit CSSValueTokenizer(const std::string &input);

    std::vector<Token> tokenize();
    Token nextToken();
    bool hasNext() const;
    void reset();

  private:
    std::string input_;
    size_t position_;
    size_t length_;

    char current_char() const;
    char peek_char(size_t offset = 1) const;
    void advance();
    void skip_whitespace();
    bool is_whitespace(char c) const;
    bool is_letter(char c) const;
    bool is_digit(char c) const;
    bool is_hex_digit(char c) const;
    bool is_identifier_start(char c) const;
    bool is_identifier_char(char c) const;

    Token consume_identifier();
    Token consume_string(char quote_char);
    Token consume_number();
    Token consume_url();
    Token consume_function(const std::string &name);
    Token consume_hash();
    std::string consume_identifier_sequence();
    void consume_escape_sequence(std::string &result);
  };
}
