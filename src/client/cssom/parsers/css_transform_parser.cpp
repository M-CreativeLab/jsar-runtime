#include "./css_transform_parser.hpp"
#include <algorithm>
#include <unordered_map>
#include <cmath>
#include <cctype>

namespace client_cssom::css_transform_parser
{
  using namespace std;
  using namespace css_value_tokenizer;

  CSSTransformParser::CSSTransformParser(const string &input)
      : input_(input)
      , tokenizer_(input)
      , current_token_index_(0)
      , is_valid_(false)
  {
    tokens_ = tokenizer_.tokenize();
  }

  vector<TransformFunction> CSSTransformParser::parse()
  {
    vector<TransformFunction> functions;
    current_token_index_ = 0;
    is_valid_ = true;
    error_message_.clear();

    // Handle 'none' case
    if (tokens_.size() == 1 && tokens_[0].type == TokenType::kIdentifier && tokens_[0].value == "none")
    {
      return functions; // Return empty list for 'none'
    }

    // Parse transform functions
    while (!isAtEnd())
    {
      auto func = parseTransformFunction();
      if (!func.has_value())
      {
        is_valid_ = false;
        return functions;
      }
      functions.push_back(func.value());

      // Skip whitespace between functions
      while (!isAtEnd() && currentToken().type == TokenType::kWhitespace)
      {
        advance();
      }
    }

    return functions;
  }

  optional<TransformFunction> CSSTransformParser::parseTransformFunction()
  {
    if (isAtEnd() || currentToken().type != TokenType::kFunction)
    {
      setError("Expected transform function");
      return nullopt;
    }

    const string &function_name = currentToken().value;
    TransformFunctionType type = getFunctionType(function_name);

    advance(); // Skip function name

    switch (type)
    {
    case TransformFunctionType::kMatrix:
      return parseMatrix();
    case TransformFunctionType::kMatrix3D:
      return parseMatrix3D();
    case TransformFunctionType::kTranslate:
      return parseTranslate();
    case TransformFunctionType::kTranslateX:
      return parseTranslateX();
    case TransformFunctionType::kTranslateY:
      return parseTranslateY();
    case TransformFunctionType::kTranslateZ:
      return parseTranslateZ();
    case TransformFunctionType::kTranslate3D:
      return parseTranslate3D();
    case TransformFunctionType::kScale:
      return parseScale();
    case TransformFunctionType::kScaleX:
      return parseScaleX();
    case TransformFunctionType::kScaleY:
      return parseScaleY();
    case TransformFunctionType::kScaleZ:
      return parseScaleZ();
    case TransformFunctionType::kScale3D:
      return parseScale3D();
    case TransformFunctionType::kRotate:
      return parseRotate();
    case TransformFunctionType::kRotateX:
      return parseRotateX();
    case TransformFunctionType::kRotateY:
      return parseRotateY();
    case TransformFunctionType::kRotateZ:
      return parseRotateZ();
    case TransformFunctionType::kRotate3D:
      return parseRotate3D();
    case TransformFunctionType::kSkew:
      return parseSkew();
    case TransformFunctionType::kSkewX:
      return parseSkewX();
    case TransformFunctionType::kSkewY:
      return parseSkewY();
    case TransformFunctionType::kPerspective:
      return parsePerspective();
    default:
      setError("Unknown transform function: " + function_name);
      return nullopt;
    }
  }

  optional<TransformFunction> CSSTransformParser::parseMatrix()
  {
    TransformFunction func(TransformFunctionType::kMatrix);

    // matrix(a, b, c, d, e, f) - 6 numbers
    for (int i = 0; i < 6; ++i)
    {
      double value;
      string unit;

      if (!consumeNumber(value, unit))
      {
        setError("Expected number in matrix()");
        return nullopt;
      }

      func.values.push_back(value);
      func.units.push_back(unit);

      if (i < 5 && !consumeComma())
      {
        setError("Expected comma in matrix()");
        return nullopt;
      }
    }

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in matrix()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseMatrix3D()
  {
    TransformFunction func(TransformFunctionType::kMatrix3D);

    // matrix3d(m11, m12, ..., m44) - 16 numbers
    for (int i = 0; i < 16; ++i)
    {
      double value;
      string unit;

      if (!consumeNumber(value, unit))
      {
        setError("Expected number in matrix3d()");
        return nullopt;
      }

      func.values.push_back(value);
      func.units.push_back(unit);

      if (i < 15 && !consumeComma())
      {
        setError("Expected comma in matrix3d()");
        return nullopt;
      }
    }

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in matrix3d()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseTranslate()
  {
    TransformFunction func(TransformFunctionType::kTranslate);

    // translate(x, y?) - 1 or 2 length/percentage values
    double value;
    string unit;

    // X value (required)
    if (!consumeLength(value, unit))
    {
      setError("Expected length/percentage in translate()");
      return nullopt;
    }
    func.values.push_back(value);
    func.units.push_back(unit);

    // Y value (optional, defaults to 0)
    if (!isAtEnd() && currentToken().type == TokenType::kComma)
    {
      advance(); // Skip comma

      if (!consumeLength(value, unit))
      {
        setError("Expected length/percentage for Y in translate()");
        return nullopt;
      }
      func.values.push_back(value);
      func.units.push_back(unit);
    }
    else
    {
      // Default Y to 0
      func.values.push_back(0.0);
      func.units.push_back("px");
    }

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in translate()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseTranslateX()
  {
    TransformFunction func(TransformFunctionType::kTranslateX);

    double value;
    string unit;

    if (!consumeLength(value, unit))
    {
      setError("Expected length/percentage in translateX()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in translateX()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseTranslateY()
  {
    TransformFunction func(TransformFunctionType::kTranslateY);

    double value;
    string unit;

    if (!consumeLength(value, unit))
    {
      setError("Expected length/percentage in translateY()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in translateY()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseTranslateZ()
  {
    TransformFunction func(TransformFunctionType::kTranslateZ);

    double value;
    string unit;

    if (!consumeLength(value, unit))
    {
      setError("Expected length in translateZ()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in translateZ()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseTranslate3D()
  {
    TransformFunction func(TransformFunctionType::kTranslate3D);

    // translate3d(x, y, z) - 3 length values
    for (int i = 0; i < 3; ++i)
    {
      double value;
      string unit;

      if (!consumeLength(value, unit))
      {
        setError("Expected length in translate3d()");
        return nullopt;
      }

      func.values.push_back(value);
      func.units.push_back(unit);

      if (i < 2 && !consumeComma())
      {
        setError("Expected comma in translate3d()");
        return nullopt;
      }
    }

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in translate3d()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseScale()
  {
    TransformFunction func(TransformFunctionType::kScale);

    // scale(x, y?) - 1 or 2 numbers
    double value;
    string unit;

    // X value (required)
    if (!consumeNumber(value, unit))
    {
      setError("Expected number in scale()");
      return nullopt;
    }
    func.values.push_back(value);
    func.units.push_back(unit);

    // Y value (optional, defaults to x)
    if (!isAtEnd() && currentToken().type == TokenType::kComma)
    {
      advance(); // Skip comma

      if (!consumeNumber(value, unit))
      {
        setError("Expected number for Y in scale()");
        return nullopt;
      }
      func.values.push_back(value);
      func.units.push_back(unit);
    }
    else
    {
      // Default Y to X value
      func.values.push_back(func.values[0]);
      func.units.push_back(func.units[0]);
    }

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in scale()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseScaleX()
  {
    TransformFunction func(TransformFunctionType::kScaleX);

    double value;
    string unit;

    if (!consumeNumber(value, unit))
    {
      setError("Expected number in scaleX()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in scaleX()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseScaleY()
  {
    TransformFunction func(TransformFunctionType::kScaleY);

    double value;
    string unit;

    if (!consumeNumber(value, unit))
    {
      setError("Expected number in scaleY()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in scaleY()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseScaleZ()
  {
    TransformFunction func(TransformFunctionType::kScaleZ);

    double value;
    string unit;

    if (!consumeNumber(value, unit))
    {
      setError("Expected number in scaleZ()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in scaleZ()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseScale3D()
  {
    TransformFunction func(TransformFunctionType::kScale3D);

    // scale3d(x, y, z) - 3 numbers
    for (int i = 0; i < 3; ++i)
    {
      double value;
      string unit;

      if (!consumeNumber(value, unit))
      {
        setError("Expected number in scale3d()");
        return nullopt;
      }

      func.values.push_back(value);
      func.units.push_back(unit);

      if (i < 2 && !consumeComma())
      {
        setError("Expected comma in scale3d()");
        return nullopt;
      }
    }

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in scale3d()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseRotate()
  {
    TransformFunction func(TransformFunctionType::kRotate);

    double value;
    string unit;

    if (!consumeAngle(value, unit))
    {
      setError("Expected angle in rotate()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in rotate()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseRotateX()
  {
    TransformFunction func(TransformFunctionType::kRotateX);

    double value;
    string unit;

    if (!consumeAngle(value, unit))
    {
      setError("Expected angle in rotateX()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in rotateX()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseRotateY()
  {
    TransformFunction func(TransformFunctionType::kRotateY);

    double value;
    string unit;

    if (!consumeAngle(value, unit))
    {
      setError("Expected angle in rotateY()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in rotateY()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseRotateZ()
  {
    TransformFunction func(TransformFunctionType::kRotateZ);

    double value;
    string unit;

    if (!consumeAngle(value, unit))
    {
      setError("Expected angle in rotateZ()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in rotateZ()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseRotate3D()
  {
    TransformFunction func(TransformFunctionType::kRotate3D);

    // rotate3d(x, y, z, angle) - 3 numbers + 1 angle
    for (int i = 0; i < 3; ++i)
    {
      double value;
      string unit;

      if (!consumeNumber(value, unit))
      {
        setError("Expected number in rotate3d()");
        return nullopt;
      }

      func.values.push_back(value);
      func.units.push_back(unit);

      if (!consumeComma())
      {
        setError("Expected comma in rotate3d()");
        return nullopt;
      }
    }

    // Angle parameter
    double value;
    string unit;
    if (!consumeAngle(value, unit))
    {
      setError("Expected angle in rotate3d()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in rotate3d()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseSkew()
  {
    TransformFunction func(TransformFunctionType::kSkew);

    // skew(x, y?) - 1 or 2 angles
    double value;
    string unit;

    // X angle (required)
    if (!consumeAngle(value, unit))
    {
      setError("Expected angle in skew()");
      return nullopt;
    }
    func.values.push_back(value);
    func.units.push_back(unit);

    // Y angle (optional, defaults to 0)
    if (!isAtEnd() && currentToken().type == TokenType::kComma)
    {
      advance(); // Skip comma

      if (!consumeAngle(value, unit))
      {
        setError("Expected angle for Y in skew()");
        return nullopt;
      }
      func.values.push_back(value);
      func.units.push_back(unit);
    }
    else
    {
      // Default Y to 0
      func.values.push_back(0.0);
      func.units.push_back("deg");
    }

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in skew()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseSkewX()
  {
    TransformFunction func(TransformFunctionType::kSkewX);

    double value;
    string unit;

    if (!consumeAngle(value, unit))
    {
      setError("Expected angle in skewX()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in skewX()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parseSkewY()
  {
    TransformFunction func(TransformFunctionType::kSkewY);

    double value;
    string unit;

    if (!consumeAngle(value, unit))
    {
      setError("Expected angle in skewY()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in skewY()");
      return nullopt;
    }

    return func;
  }

  optional<TransformFunction> CSSTransformParser::parsePerspective()
  {
    TransformFunction func(TransformFunctionType::kPerspective);

    double value;
    string unit;

    if (!consumeLength(value, unit))
    {
      setError("Expected length in perspective()");
      return nullopt;
    }

    func.values.push_back(value);
    func.units.push_back(unit);

    if (!consumeToken(TokenType::kRightParen))
    {
      setError("Expected closing parenthesis in perspective()");
      return nullopt;
    }

    return func;
  }

  // Helper methods
  bool CSSTransformParser::consumeToken(TokenType expected_type)
  {
    if (isAtEnd() || currentToken().type != expected_type)
    {
      return false;
    }
    advance();
    return true;
  }

  bool CSSTransformParser::consumeComma()
  {
    return consumeToken(TokenType::kComma);
  }

  bool CSSTransformParser::consumeNumber(double &value, string &unit)
  {
    if (isAtEnd())
    {
      return false;
    }

    const Token &token = currentToken();

    if (token.type == TokenType::kNumber)
    {
      value = token.numeric_value;
      unit = "";
      advance();
      return true;
    }
    else if (token.type == TokenType::kDimension)
    {
      value = token.numeric_value;
      unit = token.unit;
      advance();
      return true;
    }
    else if (token.type == TokenType::kPercentage)
    {
      value = token.numeric_value;
      unit = "%";
      advance();
      return true;
    }
    else if (token.type == TokenType::kIdentifier)
    {
      // Handle negative numbers tokenized as identifiers (e.g., "-10", "-1")
      const string &str = token.value;
      if (!str.empty() && str[0] == '-')
      {
        try
        {
          // Try to parse as a pure number
          double parsed_value = stod(str);
          value = parsed_value;
          unit = "";
          advance();

          // Check if next token is a decimal part (e.g., ".5" after "-0")
          if (!isAtEnd() && currentToken().type == TokenType::kNumber)
          {
            const string &next_str = currentToken().value;
            if (!next_str.empty() && next_str[0] == '.')
            {
              // Combine the integer and decimal parts
              double decimal_part = currentToken().numeric_value;
              // For negative identifiers starting with "-", always treat as negative
              if (str[0] == '-')
              {
                value = parsed_value - decimal_part;
              }
              else
              {
                value = parsed_value + decimal_part;
              }
              advance(); // Consume the decimal part
            }
          }

          return true;
        }
        catch (...)
        {
          // Not a valid number, fall through
        }
      }
    }

    return false;
  }

  bool CSSTransformParser::consumeLength(double &value, string &unit)
  {
    if (isAtEnd())
    {
      return false;
    }

    const Token &token = currentToken();

    if (token.type == TokenType::kNumber && token.numeric_value == 0.0)
    {
      // Zero is allowed without unit for lengths
      value = 0.0;
      unit = "px";
      advance();
      return true;
    }
    else if (token.type == TokenType::kDimension)
    {
      // Check if unit is a valid length unit
      const string &u = token.unit;
      if (u == "px" || u == "em" || u == "rem" || u == "vh" || u == "vw" ||
          u == "vmin" || u == "vmax" || u == "%" || u == "cm" || u == "mm" ||
          u == "in" || u == "pt" || u == "pc")
      {
        value = token.numeric_value;
        unit = u;
        advance();
        return true;
      }
    }
    else if (token.type == TokenType::kPercentage)
    {
      value = token.numeric_value;
      unit = "%";
      advance();
      return true;
    }
    else if (token.type == TokenType::kIdentifier)
    {
      // Handle negative lengths tokenized as identifiers (e.g., "-10px", "-5em")
      const string &str = token.value;
      if (!str.empty() && str[0] == '-')
      {
        // Try to extract number and unit
        size_t unit_start = 1; // Start after the minus sign
        while (unit_start < str.length() &&
               (isdigit(str[unit_start]) || str[unit_start] == '.'))
        {
          unit_start++;
        }

        if (unit_start > 1 && unit_start < str.length())
        {
          try
          {
            string number_part = str.substr(0, unit_start);
            string unit_part = str.substr(unit_start);

            // Check if unit is valid
            if (unit_part == "px" || unit_part == "em" || unit_part == "rem" ||
                unit_part == "vh" || unit_part == "vw" || unit_part == "vmin" ||
                unit_part == "vmax" || unit_part == "%" || unit_part == "cm" ||
                unit_part == "mm" || unit_part == "in" || unit_part == "pt" ||
                unit_part == "pc")
            {
              double parsed_value = stod(number_part);
              value = parsed_value;
              unit = unit_part;
              advance();
              return true;
            }
          }
          catch (...)
          {
            // Not a valid number, fall through
          }
        }
      }
    }

    return false;
  }

  bool CSSTransformParser::consumeAngle(double &value, string &unit)
  {
    if (isAtEnd())
    {
      return false;
    }

    const Token &token = currentToken();

    if (token.type == TokenType::kNumber && token.numeric_value == 0.0)
    {
      // Zero is allowed without unit for angles
      value = 0.0;
      unit = "deg";
      advance();
      return true;
    }
    else if (token.type == TokenType::kDimension)
    {
      // Check if unit is a valid angle unit
      const string &u = token.unit;
      if (u == "deg" || u == "rad" || u == "grad" || u == "turn")
      {
        value = token.numeric_value;
        unit = u;
        advance();
        return true;
      }
    }
    else if (token.type == TokenType::kIdentifier)
    {
      // Handle negative angles tokenized as identifiers (e.g., "-45deg", "-1.5rad")
      const string &str = token.value;
      if (!str.empty() && str[0] == '-')
      {
        // Try to extract number and unit
        size_t unit_start = 1; // Start after the minus sign
        while (unit_start < str.length() &&
               (isdigit(str[unit_start]) || str[unit_start] == '.'))
        {
          unit_start++;
        }

        if (unit_start > 1 && unit_start < str.length())
        {
          try
          {
            string number_part = str.substr(0, unit_start);
            string unit_part = str.substr(unit_start);

            // Check if unit is valid
            if (unit_part == "deg" || unit_part == "rad" || unit_part == "grad" || unit_part == "turn")
            {
              double parsed_value = stod(number_part);
              value = parsed_value;
              unit = unit_part;
              advance();
              return true;
            }
          }
          catch (...)
          {
            // Not a valid number, fall through
          }
        }
      }
    }

    return false;
  }

  bool CSSTransformParser::isAtEnd() const
  {
    return current_token_index_ >= tokens_.size();
  }

  const Token &CSSTransformParser::currentToken() const
  {
    static Token dummy_token(TokenType::kWhitespace);
    if (isAtEnd())
    {
      return dummy_token;
    }
    return tokens_[current_token_index_];
  }

  void CSSTransformParser::advance()
  {
    if (!isAtEnd())
    {
      current_token_index_++;
    }
  }

  void CSSTransformParser::setError(const string &message)
  {
    error_message_ = message;
    is_valid_ = false;
  }

  TransformFunctionType CSSTransformParser::getFunctionType(const string &name)
  {
    static const unordered_map<string, TransformFunctionType> function_map = {
      {"matrix", TransformFunctionType::kMatrix},
      {"matrix3d", TransformFunctionType::kMatrix3D},
      {"translate", TransformFunctionType::kTranslate},
      {"translateX", TransformFunctionType::kTranslateX},
      {"translateY", TransformFunctionType::kTranslateY},
      {"translateZ", TransformFunctionType::kTranslateZ},
      {"translate3d", TransformFunctionType::kTranslate3D},
      {"scale", TransformFunctionType::kScale},
      {"scaleX", TransformFunctionType::kScaleX},
      {"scaleY", TransformFunctionType::kScaleY},
      {"scaleZ", TransformFunctionType::kScaleZ},
      {"scale3d", TransformFunctionType::kScale3D},
      {"rotate", TransformFunctionType::kRotate},
      {"rotateX", TransformFunctionType::kRotateX},
      {"rotateY", TransformFunctionType::kRotateY},
      {"rotateZ", TransformFunctionType::kRotateZ},
      {"rotate3d", TransformFunctionType::kRotate3D},
      {"skew", TransformFunctionType::kSkew},
      {"skewX", TransformFunctionType::kSkewX},
      {"skewY", TransformFunctionType::kSkewY},
      {"perspective", TransformFunctionType::kPerspective}};

    auto it = function_map.find(name);
    if (it != function_map.end())
    {
      return it->second;
    }

    // Return a default, error will be handled by caller
    return TransformFunctionType::kMatrix;
  }
}
