#pragma once

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include "./css_value_tokenizer.hpp"

namespace client_cssom::css_transform_parser
{
  // Forward declarations
  class TransformFunction;
  
  // Enum for different transform function types
  enum class TransformFunctionType
  {
    kMatrix,
    kMatrix3D,
    kTranslate,
    kTranslateX,
    kTranslateY,
    kTranslateZ,
    kTranslate3D,
    kScale,
    kScaleX,
    kScaleY,
    kScaleZ,
    kScale3D,
    kRotate,
    kRotateX,
    kRotateY,
    kRotateZ,
    kRotate3D,
    kSkew,
    kSkewX,
    kSkewY,
    kPerspective
  };

  // Structure to hold parsed transform function data
  struct TransformFunction
  {
    TransformFunctionType type;
    std::vector<double> values;  // Numeric values
    std::vector<std::string> units;  // Units for each value
    
    TransformFunction(TransformFunctionType t) : type(t) {}
  };

  // Main parser class
  class CSSTransformParser
  {
  public:
    explicit CSSTransformParser(const std::string& input);
    
    // Parse the transform string and return list of functions
    std::vector<TransformFunction> parse();
    
    // Check if parsing was successful
    bool isValid() const { return is_valid_; }
    
    // Get error message if parsing failed
    const std::string& getError() const { return error_message_; }

  private:
    std::string input_;
    css_value_tokenizer::CSSValueTokenizer tokenizer_;
    std::vector<css_value_tokenizer::Token> tokens_;
    size_t current_token_index_;
    bool is_valid_;
    std::string error_message_;
    
    // Parse individual transform functions
    std::optional<TransformFunction> parseTransformFunction();
    std::optional<TransformFunction> parseMatrix();
    std::optional<TransformFunction> parseMatrix3D();
    std::optional<TransformFunction> parseTranslate();
    std::optional<TransformFunction> parseTranslateX();
    std::optional<TransformFunction> parseTranslateY();
    std::optional<TransformFunction> parseTranslateZ();
    std::optional<TransformFunction> parseTranslate3D();
    std::optional<TransformFunction> parseScale();
    std::optional<TransformFunction> parseScaleX();
    std::optional<TransformFunction> parseScaleY();
    std::optional<TransformFunction> parseScaleZ();
    std::optional<TransformFunction> parseScale3D();
    std::optional<TransformFunction> parseRotate();
    std::optional<TransformFunction> parseRotateX();
    std::optional<TransformFunction> parseRotateY();
    std::optional<TransformFunction> parseRotateZ();
    std::optional<TransformFunction> parseRotate3D();
    std::optional<TransformFunction> parseSkew();
    std::optional<TransformFunction> parseSkewX();
    std::optional<TransformFunction> parseSkewY();
    std::optional<TransformFunction> parsePerspective();
    
    // Helper methods
    bool consumeToken(css_value_tokenizer::TokenType expected_type);
    bool consumeComma();
    bool consumeNumber(double& value, std::string& unit);
    bool consumeLength(double& value, std::string& unit);
    bool consumeAngle(double& value, std::string& unit);
    bool isAtEnd() const;
    const css_value_tokenizer::Token& currentToken() const;
    void advance();
    void setError(const std::string& message);
    
    // Transform function name to type mapping
    static TransformFunctionType getFunctionType(const std::string& name);
  };
}