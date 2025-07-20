#include "./css_image_parser.hpp"
#include <algorithm>

namespace client_cssom::css_parser
{
  using namespace values;

  values::specified::Image CSSImageParser::parseImage(const std::string &input)
  {
    css_value_tokenizer::CSSValueTokenizer tokenizer(input);
    std::vector<css_value_tokenizer::Token> tokens = tokenizer.tokenize();

    CSSImageParser parser(tokens);
    return parser.parse();
  }

  CSSImageParser::CSSImageParser(const std::vector<css_value_tokenizer::Token> &tokens)
      : tokens_(tokens)
      , position_(0)
  {
  }

  values::specified::Image CSSImageParser::parse()
  {
    skipWhitespace();

    if (!hasNext())
    {
      return values::specified::Image::None();
    }

    const auto &token = currentToken();

    // Handle 'none' keyword
    if (token.type == css_value_tokenizer::TokenType::kIdentifier && token.value == "none")
    {
      advance();
      return values::specified::Image::None();
    }

    // Handle url() function
    if (token.type == css_value_tokenizer::TokenType::kUrl)
    {
      return parseUrl();
    }

    // Handle gradient functions and other image functions
    if (token.type == css_value_tokenizer::TokenType::kFunction)
    {
      if (token.value == "src")
      {
        return parseSrc();
      }
      else if (token.value == "image-set")
      {
        return parseImageSet();
      }
      else
      {
        return parseGradient(token.value);
      }
    }

    // Default to none for unrecognized input
    return values::specified::Image::None();
  }

  values::specified::Image CSSImageParser::parseUrl()
  {
    const auto &token = currentToken();
    if (token.type != css_value_tokenizer::TokenType::kUrl)
    {
      return values::specified::Image::None();
    }

    advance();
    return values::specified::Image::Url(token.value);
  }

  values::specified::Image CSSImageParser::parseSrc()
  {
    if (!consumeFunction("src"))
    {
      return values::specified::Image::None();
    }

    skipWhitespace();

    // src() function expects a URL string
    if (hasNext() && currentToken().type == css_value_tokenizer::TokenType::kString)
    {
      std::string url = currentToken().value;
      advance();

      skipWhitespace();

      // Consume closing parenthesis
      if (hasNext() && currentToken().type == css_value_tokenizer::TokenType::kRightParen)
      {
        advance();
        return values::specified::Image::Url(url);
      }
    }

    return values::specified::Image::None();
  }

  values::specified::Image CSSImageParser::parseImageSet()
  {
    if (!consumeFunction("image-set"))
    {
      return values::specified::Image::None();
    }

    skipWhitespace();

    // For now, just parse the first image option in the image-set
    // A full implementation would parse all options and resolutions
    if (hasNext())
    {
      const auto &token = currentToken();

      // Parse src() function inside image-set
      if (token.type == css_value_tokenizer::TokenType::kFunction && token.value == "src")
      {
        auto image = parseSrc();

        // Skip any resolution descriptors and additional options for now
        while (hasNext() && currentToken().type != css_value_tokenizer::TokenType::kRightParen)
        {
          advance();
        }

        // Consume closing parenthesis
        if (hasNext() && currentToken().type == css_value_tokenizer::TokenType::kRightParen)
        {
          advance();
        }

        return image;
      }

      // Parse url() function inside image-set
      if (token.type == css_value_tokenizer::TokenType::kUrl)
      {
        auto image = parseUrl();

        // Skip any resolution descriptors and additional options for now
        while (hasNext() && currentToken().type != css_value_tokenizer::TokenType::kRightParen)
        {
          advance();
        }

        // Consume closing parenthesis
        if (hasNext() && currentToken().type == css_value_tokenizer::TokenType::kRightParen)
        {
          advance();
        }

        return image;
      }
    }

    return values::specified::Image::None();
  }

  values::specified::Image CSSImageParser::parseGradient(const std::string &function_name)
  {
    if (function_name == "linear-gradient")
    {
      return parseLinearGradient(false);
    }
    else if (function_name == "repeating-linear-gradient")
    {
      return parseLinearGradient(true);
    }
    else if (function_name == "radial-gradient")
    {
      return parseRadialGradient(false);
    }
    else if (function_name == "repeating-radial-gradient")
    {
      return parseRadialGradient(true);
    }

    return values::specified::Image::None();
  }

  values::specified::Image CSSImageParser::parseLinearGradient(bool repeating)
  {
    if (!consumeFunction("linear-gradient") && !consumeFunction("repeating-linear-gradient"))
    {
      return values::specified::Image::None();
    }

    generics::GenericGradient<specified::NoCalcLength, specified::LengthPercentage, specified::Color>::LinearGradient linearGrad;
    linearGrad.direction = generics::LineDirection::kToBottom; // Default direction (180deg)

    skipWhitespace();

    // Parse optional direction
    bool hasDirection = false;
    if (hasNext())
    {
      // Try to parse direction keywords
      const auto &token = currentToken();
      if (token.type == css_value_tokenizer::TokenType::kIdentifier)
      {
        if (token.value == "to")
        {
          advance();
          skipWhitespace();

          if (hasNext() && currentToken().type == css_value_tokenizer::TokenType::kIdentifier)
          {
            std::string direction_keyword = currentToken().value;
            advance();

            // Check for compound directions (e.g., "to top left")
            if (hasNext() && currentToken().type == css_value_tokenizer::TokenType::kIdentifier)
            {
              direction_keyword += " " + currentToken().value;
              advance();
            }

            // Map direction keywords
            if (direction_keyword == "right")
            {
              linearGrad.direction = generics::LineDirection::kToRight;
              hasDirection = true;
            }
            else if (direction_keyword == "left")
            {
              linearGrad.direction = generics::LineDirection::kToLeft;
              hasDirection = true;
            }
            else if (direction_keyword == "top")
            {
              linearGrad.direction = generics::LineDirection::kToTop;
              hasDirection = true;
            }
            else if (direction_keyword == "bottom")
            {
              linearGrad.direction = generics::LineDirection::kToBottom;
              hasDirection = true;
            }
            else if (direction_keyword == "top left")
            {
              linearGrad.direction = generics::LineDirection::kToTopLeft;
              hasDirection = true;
            }
            else if (direction_keyword == "top right")
            {
              linearGrad.direction = generics::LineDirection::kToTopRight;
              hasDirection = true;
            }
            else if (direction_keyword == "bottom left")
            {
              linearGrad.direction = generics::LineDirection::kToBottomLeft;
              hasDirection = true;
            }
            else if (direction_keyword == "bottom right")
            {
              linearGrad.direction = generics::LineDirection::kToBottomRight;
              hasDirection = true;
            }
          }
        }
        // TODO: Parse angle values (e.g., "45deg", "0.25turn", "1.5708rad")
      }

      // If we parsed a direction, expect a comma
      if (hasDirection)
      {
        skipWhitespace();
        if (hasNext() && currentToken().type == css_value_tokenizer::TokenType::kComma)
        {
          advance();
          skipWhitespace();
        }
      }
    }

    // Parse color stops
    linearGrad.items = parseColorStops();

    // Create the gradient
    specified::Gradient gradient(linearGrad);
    gradient.repeating = repeating;

    specified::Image image;
    image.emplace<specified::Gradient>(gradient);
    return image;
  }

  values::specified::Image CSSImageParser::parseRadialGradient(bool repeating)
  {
    if (!consumeFunction("radial-gradient") && !consumeFunction("repeating-radial-gradient"))
    {
      return values::specified::Image::None();
    }

    generics::GenericGradient<specified::NoCalcLength, specified::LengthPercentage, specified::Color>::RadialGradient radialGrad;
    radialGrad.shape = generics::GenericGradient<specified::NoCalcLength, specified::LengthPercentage, specified::Color>::RadialGradient::Shape::kEllipse;
    radialGrad.size = generics::GenericGradient<specified::NoCalcLength, specified::LengthPercentage, specified::Color>::RadialGradient::Size::kFarthestCorner;

    skipWhitespace();

    // Parse optional shape/size
    bool hasShapeSize = parseRadialGradientShape(radialGrad);

    if (hasShapeSize)
    {
      skipWhitespace();
      if (hasNext() && currentToken().type == css_value_tokenizer::TokenType::kComma)
      {
        advance();
        skipWhitespace();
      }
    }

    // Parse color stops
    radialGrad.items = parseColorStops();

    // Create the gradient
    specified::Gradient gradient(radialGrad);
    gradient.repeating = repeating;

    specified::Image image;
    image.emplace<specified::Gradient>(gradient);
    return image;
  }

  bool CSSImageParser::parseRadialGradientShape(generics::GenericGradient<specified::NoCalcLength,
                                                                          specified::LengthPercentage,
                                                                          specified::Color>::RadialGradient &radial)
  {
    bool modified = false;

    while (hasNext() && currentToken().type == css_value_tokenizer::TokenType::kIdentifier)
    {
      const std::string &keyword = currentToken().value;

      if (keyword == "circle")
      {
        radial.shape = generics::GenericGradient<specified::NoCalcLength, specified::LengthPercentage, specified::Color>::RadialGradient::Shape::kCircle;
        advance();
        modified = true;
      }
      else if (keyword == "ellipse")
      {
        radial.shape = generics::GenericGradient<specified::NoCalcLength, specified::LengthPercentage, specified::Color>::RadialGradient::Shape::kEllipse;
        advance();
        modified = true;
      }
      else if (keyword == "closest-side")
      {
        radial.size = generics::GenericGradient<specified::NoCalcLength, specified::LengthPercentage, specified::Color>::RadialGradient::Size::kClosestSide;
        advance();
        modified = true;
      }
      else if (keyword == "closest-corner")
      {
        radial.size = generics::GenericGradient<specified::NoCalcLength, specified::LengthPercentage, specified::Color>::RadialGradient::Size::kClosestCorner;
        advance();
        modified = true;
      }
      else if (keyword == "farthest-side")
      {
        radial.size = generics::GenericGradient<specified::NoCalcLength, specified::LengthPercentage, specified::Color>::RadialGradient::Size::kFarthestSide;
        advance();
        modified = true;
      }
      else if (keyword == "farthest-corner")
      {
        radial.size = generics::GenericGradient<specified::NoCalcLength, specified::LengthPercentage, specified::Color>::RadialGradient::Size::kFarthestCorner;
        advance();
        modified = true;
      }
      else
      {
        break; // Not a shape/size keyword
      }

      skipWhitespace();
    }

    return modified;
  }

  std::vector<generics::GenericGradientItem<specified::Color, specified::LengthPercentage>> CSSImageParser::parseColorStops()
  {
    std::vector<generics::GenericGradientItem<specified::Color, specified::LengthPercentage>> colorStops;

    while (hasNext())
    {
      auto colorStop = parseColorStop();
      colorStops.push_back(colorStop);

      skipWhitespace();

      // Check for comma separator
      if (hasNext() && currentToken().type == css_value_tokenizer::TokenType::kComma)
      {
        advance();
        skipWhitespace();
      }
      else
      {
        break;
      }
    }

    return colorStops;
  }

  generics::GenericGradientItem<specified::Color, specified::LengthPercentage> CSSImageParser::parseColorStop()
  {
    generics::GenericGradientItem<specified::Color, specified::LengthPercentage> colorStop;

    // For now, create simple color stops
    // TODO: Implement proper color parsing and position parsing
    colorStop.type = generics::GenericGradientItem<specified::Color, specified::LengthPercentage>::kSimpleColorStop;

    // Skip color name tokens for now
    if (hasNext() && currentToken().type == css_value_tokenizer::TokenType::kIdentifier)
    {
      advance();
    }

    return colorStop;
  }

  bool CSSImageParser::consumeToken(css_value_tokenizer::TokenType expected_type)
  {
    if (hasNext() && currentToken().type == expected_type)
    {
      advance();
      return true;
    }
    return false;
  }

  bool CSSImageParser::consumeIdentifier(const std::string &expected_value)
  {
    if (hasNext() && currentToken().type == css_value_tokenizer::TokenType::kIdentifier &&
        currentToken().value == expected_value)
    {
      advance();
      return true;
    }
    return false;
  }

  bool CSSImageParser::consumeFunction(const std::string &expected_name)
  {
    if (hasNext() && currentToken().type == css_value_tokenizer::TokenType::kFunction &&
        currentToken().value == expected_name)
    {
      advance();
      return true;
    }
    return false;
  }

  bool CSSImageParser::hasNext() const
  {
    return position_ < tokens_.size();
  }

  const css_value_tokenizer::Token &CSSImageParser::currentToken() const
  {
    static css_value_tokenizer::Token dummy_token(css_value_tokenizer::TokenType::kWhitespace);
    return hasNext() ? tokens_[position_] : dummy_token;
  }

  const css_value_tokenizer::Token &CSSImageParser::peekToken(size_t offset) const
  {
    static css_value_tokenizer::Token dummy_token(css_value_tokenizer::TokenType::kWhitespace);
    size_t peek_pos = position_ + offset;
    return peek_pos < tokens_.size() ? tokens_[peek_pos] : dummy_token;
  }

  void CSSImageParser::advance()
  {
    if (hasNext())
    {
      position_++;
    }
  }

  void CSSImageParser::skipWhitespace()
  {
    while (hasNext() && currentToken().type == css_value_tokenizer::TokenType::kWhitespace)
    {
      advance();
    }
  }
}