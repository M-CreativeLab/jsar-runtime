#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>
#include "./css_tokenizer.hpp"

namespace client_cssom::css_parser
{
  using namespace css_tokenizer;

  // Forward declarations
  class CSSRule;
  class CSSStyleRule;
  class CSSKeyframeRule;
  class CSSFontFaceRule;
  class CSSMediaRule;
  class CSSImportRule;

  // CSS Rule types
  enum class CSSRuleType
  {
    kStyle,
    kKeyframe,
    kFontFace,
    kMedia,
    kImport
  };

  // Base class for all CSS rules
  class CSSRule
  {
  public:
    virtual ~CSSRule() = default;
    virtual CSSRuleType getType() const = 0;
    virtual std::string toCSSText() const = 0;
  };

  // Style rule (selector { declarations })
  class CSSStyleRule : public CSSRule
  {
  public:
    std::string selector_text;
    std::unordered_map<std::string, std::string> declarations;

    CSSRuleType getType() const override
    {
      return CSSRuleType::kStyle;
    }
    std::string toCSSText() const override;
  };

  // Keyframe rule (@keyframes)
  class CSSKeyframeRule : public CSSRule
  {
  public:
    std::string name;
    std::vector<std::string> keyframes; // "0%", "50%", "100%", "from", "to"
    std::unordered_map<std::string, std::string> declarations;

    CSSRuleType getType() const override
    {
      return CSSRuleType::kKeyframe;
    }
    std::string toCSSText() const override;
  };

  // Font face rule (@font-face)
  class CSSFontFaceRule : public CSSRule
  {
  public:
    std::unordered_map<std::string, std::string> declarations;

    CSSRuleType getType() const override
    {
      return CSSRuleType::kFontFace;
    }
    std::string toCSSText() const override;
  };

  // Media rule (@media)
  class CSSMediaRule : public CSSRule
  {
  public:
    std::string media_query;
    std::vector<std::unique_ptr<CSSRule>> rules;

    CSSRuleType getType() const override
    {
      return CSSRuleType::kMedia;
    }
    std::string toCSSText() const override;
  };

  // Import rule (@import)
  class CSSImportRule : public CSSRule
  {
  public:
    std::string url;
    std::string media_query;

    CSSRuleType getType() const override
    {
      return CSSRuleType::kImport;
    }
    std::string toCSSText() const override;
  };

  // Style declaration result for parsing individual declarations
  struct StyleDeclaration
  {
    std::unordered_map<std::string, std::string> properties;
    bool valid = true;
    std::string error_message;
  };

  // Main CSS parser class
  class CSSParser
  {
  public:
    explicit CSSParser(const std::string &input);

    // Parse complete stylesheet
    std::vector<std::unique_ptr<CSSRule>> parseStylesheet();

    // Parse single style declaration (e.g., "color: red; font-size: 14px")
    StyleDeclaration parseStyleDeclaration(const std::string &declaration_string);

    // Error handling
    bool isValid() const
    {
      return is_valid_;
    }
    const std::string &getError() const
    {
      return error_message_;
    }

  private:
    std::string input_;
    css_tokenizer::CSSTokenizer tokenizer_;
    std::vector<Token> tokens_;
    size_t current_token_index_;
    bool is_valid_;
    std::string error_message_;

    // Parsing methods
    std::unique_ptr<CSSRule> parseRule();
    std::unique_ptr<CSSStyleRule> parseStyleRule();
    std::unique_ptr<CSSKeyframeRule> parseKeyframeRule();
    std::unique_ptr<CSSFontFaceRule> parseFontFaceRule();
    std::unique_ptr<CSSMediaRule> parseMediaRule();
    std::unique_ptr<CSSImportRule> parseImportRule();

    // Declaration parsing
    std::unordered_map<std::string, std::string> parseDeclarations();
    bool parseDeclaration(std::string &property, std::string &value);

    // Selector parsing (simplified)
    std::string parseSelector();

    // Value parsing helpers
    std::string parseValue();
    std::string parseMediaQuery();
    std::string parseUrl();

    // Token consumption helpers
    bool consumeToken(TokenType expected_type);
    bool consumeIdentifier(const std::string &expected_value);
    bool hasNext() const;
    const Token &currentToken() const;
    const Token &peekToken(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    void skipTo(TokenType token_type);
    bool isAtEnd() const;

    // Error handling
    void setError(const std::string &message);
  };
}