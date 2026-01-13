#pragma once

#include <string>
#include <vector>
#include <variant>

namespace client_cssom::css_tokenizer
{
  enum class TokenType
  {
    // Basic tokens from CSS Syntax Module Level 3
    kIdentifier,
    kFunction,
    kAtKeyword, // @media, @keyframes, etc.
    kHash,      // #id, #color
    kString,
    kBadString,
    kUrl,
    kBadUrl,
    kDelimiter, // Single character delimiters
    kNumber,
    kPercentage,
    kDimension,
    kIncludeMatch,   // ~=
    kDashMatch,      // |=
    kPrefixMatch,    // ^=
    kSuffixMatch,    // $=
    kSubstringMatch, // *=
    kColumn,         // ||
    kWhitespace,
    kCDO,                // <!--
    kCDC,                // -->
    kColon,              // :
    kSemicolon,          // ;
    kComma,              // ,
    kLeftSquareBracket,  // [
    kRightSquareBracket, // ]
    kLeftParen,          // (
    kRightParen,         // )
    kLeftCurlyBracket,   // {
    kRightCurlyBracket,  // }
    kComment,            // /* comment */
    kEOF
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

  class CSSTokenizer
  {
  public:
    explicit CSSTokenizer(const std::string &input);

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
    bool is_non_printable(char c) const;

    Token consume_identifier();
    Token consume_string(char quote_char);
    Token consume_number();
    Token consume_url();
    Token consume_function(const std::string &name);
    Token consume_at_keyword();
    Token consume_hash();
    Token consume_comment();
    Token consume_numeric();
    std::string consume_identifier_sequence();
    void consume_escape_sequence(std::string &result);
    bool would_start_identifier() const;
    bool would_start_number() const;
  };
}