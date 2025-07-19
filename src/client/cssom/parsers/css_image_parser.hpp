#pragma once

#include "./css_value_tokenizer.hpp"
#include "../values/specified/image.hpp"
#include "../values/specified/color.hpp"
#include "../values/specified/length.hpp"
#include "../values/specified/angle.hpp"

namespace client_cssom::css_parser
{
  class CSSImageParser
  {
  public:
    static values::specified::Image parseImage(const std::string &input);

  private:
    explicit CSSImageParser(const std::vector<css_value_tokenizer::Token> &tokens);

    values::specified::Image parse();
    values::specified::Image parseUrl();
    values::specified::Image parseGradient(const std::string &function_name);
    values::specified::Image parseLinearGradient(bool repeating = false);
    values::specified::Image parseRadialGradient(bool repeating = false);

    // Gradient component parsing
    bool parseLinearGradientDirection(values::generics::LineDirection &direction);
    bool parseAngle(values::specified::Angle &angle);
    bool parseRadialGradientShape(values::generics::GenericGradient<values::specified::NoCalcLength,
                                                                    values::specified::LengthPercentage,
                                                                    values::specified::Color>::RadialGradient &radial);
    std::vector<values::generics::GenericGradientItem<values::specified::Color,
                                                      values::specified::LengthPercentage>>
    parseColorStops();
    values::generics::GenericGradientItem<values::specified::Color,
                                          values::specified::LengthPercentage>
    parseColorStop();

    // Helper methods
    bool consumeToken(css_value_tokenizer::TokenType expected_type);
    bool consumeIdentifier(const std::string &expected_value);
    bool consumeFunction(const std::string &expected_name);
    bool hasNext() const;
    const css_value_tokenizer::Token &currentToken() const;
    const css_value_tokenizer::Token &peekToken(size_t offset = 1) const;
    void advance();
    void skipWhitespace();

    std::vector<css_value_tokenizer::Token> tokens_;
    size_t position_;
  };
}