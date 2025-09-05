#include "./css_parser.hpp"
#include <algorithm>
#include <sstream>

namespace client_cssom::css_parser
{
  using namespace std;
  using namespace css_tokenizer;

  // CSSStyleRule implementation
  string CSSStyleRule::toCSSText() const
  {
    stringstream ss;
    ss << selector_text << " {";

    for (const auto &[property, value] : declarations)
    {
      ss << " " << property << ": " << value << ";";
    }

    ss << " }";
    return ss.str();
  }

  // CSSKeyframeRule implementation
  string CSSKeyframeRule::toCSSText() const
  {
    stringstream ss;
    ss << "@keyframes " << name << " {";

    for (const auto &keyframe : keyframes)
    {
      ss << " " << keyframe << " {";
      for (const auto &[property, value] : declarations)
      {
        ss << " " << property << ": " << value << ";";
      }
      ss << " }";
    }

    ss << " }";
    return ss.str();
  }

  // CSSFontFaceRule implementation
  string CSSFontFaceRule::toCSSText() const
  {
    stringstream ss;
    ss << "@font-face {";

    for (const auto &[property, value] : declarations)
    {
      ss << " " << property << ": " << value << ";";
    }

    ss << " }";
    return ss.str();
  }

  // CSSMediaRule implementation
  string CSSMediaRule::toCSSText() const
  {
    stringstream ss;
    ss << "@media " << media_query << " {";

    for (const auto &rule : rules)
    {
      ss << " " << rule->toCSSText();
    }

    ss << " }";
    return ss.str();
  }

  // CSSImportRule implementation
  string CSSImportRule::toCSSText() const
  {
    stringstream ss;
    ss << "@import " << url;
    if (!media_query.empty())
    {
      ss << " " << media_query;
    }
    ss << ";";
    return ss.str();
  }

  // CSSParser implementation
  CSSParser::CSSParser(const string &input)
      : input_(input)
      , tokenizer_(input)
      , current_token_index_(0)
      , is_valid_(false)
  {
    tokens_ = tokenizer_.tokenize();
  }

  vector<unique_ptr<CSSRule>> CSSParser::parseStylesheet()
  {
    vector<unique_ptr<CSSRule>> rules;
    current_token_index_ = 0;
    is_valid_ = true;
    error_message_.clear();

    skipWhitespace();

    while (!isAtEnd())
    {
      auto rule = parseRule();
      if (rule)
      {
        rules.push_back(move(rule));
      }
      else if (!is_valid_)
      {
        break; // Stop on error
      }

      skipWhitespace();
    }

    return rules;
  }

  StyleDeclaration CSSParser::parseStyleDeclaration(const string &declaration_string)
  {
    StyleDeclaration result;

    // Create a new tokenizer for just this declaration
    CSSTokenizer decl_tokenizer(declaration_string);
    auto decl_tokens = decl_tokenizer.tokenize();

    // Temporarily replace our tokens
    auto saved_tokens = move(tokens_);
    auto saved_index = current_token_index_;

    tokens_ = move(decl_tokens);
    current_token_index_ = 0;
    is_valid_ = true;
    error_message_.clear();

    result.properties = parseDeclarations();
    result.valid = is_valid_;
    result.error_message = error_message_;

    // Restore original tokens
    tokens_ = move(saved_tokens);
    current_token_index_ = saved_index;

    return result;
  }

  unique_ptr<CSSRule> CSSParser::parseRule()
  {
    skipWhitespace();

    if (isAtEnd())
      return nullptr;

    const auto &token = currentToken();

    // At-rules
    if (token.type == TokenType::kAtKeyword)
    {
      string at_rule = token.value;
      transform(at_rule.begin(), at_rule.end(), at_rule.begin(), ::tolower);

      if (at_rule == "@keyframes")
      {
        return parseKeyframeRule();
      }
      else if (at_rule == "@font-face")
      {
        return parseFontFaceRule();
      }
      else if (at_rule == "@media")
      {
        return parseMediaRule();
      }
      else if (at_rule == "@import")
      {
        return parseImportRule();
      }
      else
      {
        // Skip unknown at-rule
        skipTo(TokenType::kSemicolon);
        if (!isAtEnd())
          advance();
        return nullptr;
      }
    }
    else
    {
      // Regular style rule
      return parseStyleRule();
    }
  }

  unique_ptr<CSSStyleRule> CSSParser::parseStyleRule()
  {
    auto rule = make_unique<CSSStyleRule>();

    // Parse selector
    rule->selector_text = parseSelector();

    if (!consumeToken(TokenType::kLeftCurlyBracket))
    {
      setError("Expected '{' after selector");
      return nullptr;
    }

    // Parse declarations
    rule->declarations = parseDeclarations();

    if (!consumeToken(TokenType::kRightCurlyBracket))
    {
      setError("Expected '}' after declarations");
      return nullptr;
    }

    return rule;
  }

  unique_ptr<CSSKeyframeRule> CSSParser::parseKeyframeRule()
  {
    auto rule = make_unique<CSSKeyframeRule>();

    advance(); // Consume @keyframes
    skipWhitespace();

    // Parse keyframes name
    if (currentToken().type == TokenType::kIdentifier)
    {
      rule->name = currentToken().value;
      advance();
    }
    else
    {
      setError("Expected keyframes name");
      return nullptr;
    }

    skipWhitespace();

    if (!consumeToken(TokenType::kLeftCurlyBracket))
    {
      setError("Expected '{' after keyframes name");
      return nullptr;
    }

    // For simplicity, parse entire keyframes content as one declaration block
    // A full implementation would parse individual keyframe blocks
    rule->declarations = parseDeclarations();

    if (!consumeToken(TokenType::kRightCurlyBracket))
    {
      setError("Expected '}' after keyframes");
      return nullptr;
    }

    return rule;
  }

  unique_ptr<CSSFontFaceRule> CSSParser::parseFontFaceRule()
  {
    auto rule = make_unique<CSSFontFaceRule>();

    advance(); // Consume @font-face
    skipWhitespace();

    if (!consumeToken(TokenType::kLeftCurlyBracket))
    {
      setError("Expected '{' after @font-face");
      return nullptr;
    }

    rule->declarations = parseDeclarations();

    if (!consumeToken(TokenType::kRightCurlyBracket))
    {
      setError("Expected '}' after font-face declarations");
      return nullptr;
    }

    return rule;
  }

  unique_ptr<CSSMediaRule> CSSParser::parseMediaRule()
  {
    auto rule = make_unique<CSSMediaRule>();

    advance(); // Consume @media
    skipWhitespace();

    // Parse media query
    rule->media_query = parseMediaQuery();

    skipWhitespace();

    if (!consumeToken(TokenType::kLeftCurlyBracket))
    {
      setError("Expected '{' after media query");
      return nullptr;
    }

    // Parse nested rules
    skipWhitespace();
    while (!isAtEnd() && currentToken().type != TokenType::kRightCurlyBracket)
    {
      auto nested_rule = parseRule();
      if (nested_rule)
      {
        rule->rules.push_back(move(nested_rule));
      }
      skipWhitespace();
    }

    if (!consumeToken(TokenType::kRightCurlyBracket))
    {
      setError("Expected '}' after media rules");
      return nullptr;
    }

    return rule;
  }

  unique_ptr<CSSImportRule> CSSParser::parseImportRule()
  {
    auto rule = make_unique<CSSImportRule>();

    advance(); // Consume @import
    skipWhitespace();

    // Parse URL
    rule->url = parseUrl();

    skipWhitespace();

    // Optional media query
    if (!isAtEnd() && currentToken().type != TokenType::kSemicolon)
    {
      rule->media_query = parseMediaQuery();
    }

    if (!consumeToken(TokenType::kSemicolon))
    {
      setError("Expected ';' after import rule");
      return nullptr;
    }

    return rule;
  }

  unordered_map<string, string> CSSParser::parseDeclarations()
  {
    unordered_map<string, string> declarations;

    skipWhitespace();

    while (!isAtEnd() && currentToken().type != TokenType::kRightCurlyBracket)
    {
      string property, value;
      if (parseDeclaration(property, value))
      {
        declarations[property] = value;
      }

      skipWhitespace();

      // Optional semicolon
      if (!isAtEnd() && currentToken().type == TokenType::kSemicolon)
      {
        advance();
        skipWhitespace();
      }
    }

    return declarations;
  }

  bool CSSParser::parseDeclaration(string &property, string &value)
  {
    skipWhitespace();

    // Parse property name
    if (currentToken().type != TokenType::kIdentifier)
    {
      // Skip invalid declaration
      skipTo(TokenType::kSemicolon);
      return false;
    }

    property = currentToken().value;
    advance();

    skipWhitespace();

    if (!consumeToken(TokenType::kColon))
    {
      // Skip invalid declaration
      skipTo(TokenType::kSemicolon);
      return false;
    }

    skipWhitespace();

    // Parse value
    value = parseValue();

    return !property.empty() && !value.empty();
  }

  string CSSParser::parseSelector()
  {
    string selector;

    while (!isAtEnd() && currentToken().type != TokenType::kLeftCurlyBracket)
    {
      const auto &token = currentToken();

      if (token.type == TokenType::kWhitespace)
      {
        if (!selector.empty() && selector.back() != ' ')
        {
          selector += " ";
        }
      }
      else
      {
        selector += token.value;
      }

      advance();
    }

    // Trim trailing whitespace
    while (!selector.empty() && selector.back() == ' ')
    {
      selector.pop_back();
    }

    return selector;
  }

  string CSSParser::parseValue()
  {
    string value;
    int paren_depth = 0;
    bool first_token = true;

    while (!isAtEnd())
    {
      const auto &token = currentToken();

      if (token.type == TokenType::kSemicolon && paren_depth == 0)
      {
        break;
      }

      if (token.type == TokenType::kRightCurlyBracket && paren_depth == 0)
      {
        break;
      }

      // Handle function tokens - they consume the opening parenthesis
      if (token.type == TokenType::kFunction)
      {
        if (!value.empty() && value.back() != ' ')
        {
          value += " ";
        }
        value += token.value + "(";
        paren_depth++;
        first_token = false;
      }
      else if (token.type == TokenType::kLeftParen)
      {
        paren_depth++;
        value += token.value;
        first_token = false;
      }
      else if (token.type == TokenType::kRightParen)
      {
        paren_depth--;
        value += token.value;
        first_token = false;
      }
      else if (token.type == TokenType::kWhitespace)
      {
        if (!value.empty() && value.back() != ' ' && !first_token)
        {
          value += " ";
        }
      }
      else
      {
        if (!value.empty() && value.back() != ' ' && token.type != TokenType::kComma && 
            !first_token && value.back() != '(' && value.back() != ',')
        {
          value += " ";
        }
        value += token.value;
        first_token = false;
      }

      advance();
    }

    // Trim trailing whitespace
    while (!value.empty() && value.back() == ' ')
    {
      value.pop_back();
    }

    return value;
  }

  string CSSParser::parseMediaQuery()
  {
    string media_query;

    while (!isAtEnd() && currentToken().type != TokenType::kLeftCurlyBracket)
    {
      const auto &token = currentToken();

      if (token.type == TokenType::kWhitespace)
      {
        if (!media_query.empty() && media_query.back() != ' ')
        {
          media_query += " ";
        }
      }
      else
      {
        media_query += token.value;
      }

      advance();
    }

    // Trim trailing whitespace
    while (!media_query.empty() && media_query.back() == ' ')
    {
      media_query.pop_back();
    }

    return media_query;
  }

  string CSSParser::parseUrl()
  {
    string url;

    if (currentToken().type == TokenType::kUrl)
    {
      url = currentToken().value;
      advance();
    }
    else if (currentToken().type == TokenType::kString)
    {
      url = "\"" + currentToken().value + "\"";
      advance();
    }
    else
    {
      // Parse as identifier or function
      url = currentToken().value;
      advance();
    }

    return url;
  }

  bool CSSParser::consumeToken(TokenType expected_type)
  {
    if (isAtEnd() || currentToken().type != expected_type)
    {
      return false;
    }
    advance();
    return true;
  }

  bool CSSParser::consumeIdentifier(const string &expected_value)
  {
    if (isAtEnd() || currentToken().type != TokenType::kIdentifier ||
        currentToken().value != expected_value)
    {
      return false;
    }
    advance();
    return true;
  }

  bool CSSParser::hasNext() const
  {
    return current_token_index_ < tokens_.size();
  }

  const Token &CSSParser::currentToken() const
  {
    static Token dummy_token(TokenType::kEOF);
    if (isAtEnd())
      return dummy_token;
    return tokens_[current_token_index_];
  }

  const Token &CSSParser::peekToken(size_t offset) const
  {
    static Token dummy_token(TokenType::kEOF);
    size_t peek_index = current_token_index_ + offset;
    if (peek_index >= tokens_.size())
      return dummy_token;
    return tokens_[peek_index];
  }

  void CSSParser::advance()
  {
    if (current_token_index_ < tokens_.size())
    {
      current_token_index_++;
    }
  }

  void CSSParser::skipWhitespace()
  {
    while (!isAtEnd() && currentToken().type == TokenType::kWhitespace)
    {
      advance();
    }
  }

  void CSSParser::skipTo(TokenType token_type)
  {
    while (!isAtEnd() && currentToken().type != token_type)
    {
      advance();
    }
  }

  bool CSSParser::isAtEnd() const
  {
    return current_token_index_ >= tokens_.size() ||
           (current_token_index_ < tokens_.size() && tokens_[current_token_index_].type == TokenType::kEOF);
  }

  void CSSParser::setError(const string &message)
  {
    is_valid_ = false;
    error_message_ = message;
  }
}