#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/cssom/parsers/css_transform_parser.hpp>
#include <client/cssom/values/specified/transform.hpp>
#include <client/cssom/style_traits.hpp>

using namespace client_cssom::css_transform_parser;
using namespace client_cssom::values::specified;
using namespace client_cssom;

TEST_CASE("CSSTransformParser basic functionality", "[css-transform-parser]")
{
  SECTION("Parse none")
  {
    CSSTransformParser parser("none");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.empty());
  }
  
  SECTION("Parse simple translate")
  {
    CSSTransformParser parser("translateX(10px)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kTranslateX);
    REQUIRE(functions[0].values.size() == 1);
    REQUIRE(functions[0].values[0] == 10.0);
    REQUIRE(functions[0].units[0] == "px");
  }
  
  SECTION("Parse translate with two values")
  {
    CSSTransformParser parser("translate(10px, 20px)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kTranslate);
    REQUIRE(functions[0].values.size() == 2);
    REQUIRE(functions[0].values[0] == 10.0);
    REQUIRE(functions[0].values[1] == 20.0);
    REQUIRE(functions[0].units[0] == "px");
    REQUIRE(functions[0].units[1] == "px");
  }
  
  SECTION("Parse translate with one value (should default Y to 0)")
  {
    CSSTransformParser parser("translate(10px)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kTranslate);
    REQUIRE(functions[0].values.size() == 2);
    REQUIRE(functions[0].values[0] == 10.0);
    REQUIRE(functions[0].values[1] == 0.0);
    REQUIRE(functions[0].units[0] == "px");
    REQUIRE(functions[0].units[1] == "px");
  }
}

TEST_CASE("CSSTransformParser rotation functions", "[css-transform-parser]")
{
  SECTION("Parse rotate")
  {
    CSSTransformParser parser("rotate(45deg)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kRotate);
    REQUIRE(functions[0].values.size() == 1);
    REQUIRE(functions[0].values[0] == 45.0);
    REQUIRE(functions[0].units[0] == "deg");
  }
  
  SECTION("Parse rotateX")
  {
    CSSTransformParser parser("rotateX(90deg)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kRotateX);
    REQUIRE(functions[0].values.size() == 1);
    REQUIRE(functions[0].values[0] == 90.0);
    REQUIRE(functions[0].units[0] == "deg");
  }
  
  SECTION("Parse rotate with radians")
  {
    CSSTransformParser parser("rotate(1.57rad)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kRotate);
    REQUIRE(functions[0].values.size() == 1);
    // REQUIRE(functions[0].values[0] == 1.57f);
    REQUIRE(functions[0].units[0] == "rad");
  }
  
  SECTION("Parse rotate3d")
  {
    CSSTransformParser parser("rotate3d(1, 0, 0, 45deg)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kRotate3D);
    REQUIRE(functions[0].values.size() == 4);
    REQUIRE(functions[0].values[0] == 1.0);
    REQUIRE(functions[0].values[1] == 0.0);
    REQUIRE(functions[0].values[2] == 0.0);
    REQUIRE(functions[0].values[3] == 45.0);
    REQUIRE(functions[0].units[3] == "deg");
  }
}

TEST_CASE("CSSTransformParser scale functions", "[css-transform-parser]")
{
  SECTION("Parse scale with one value")
  {
    CSSTransformParser parser("scale(2)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kScale);
    REQUIRE(functions[0].values.size() == 2);
    REQUIRE(functions[0].values[0] == 2.0);
    REQUIRE(functions[0].values[1] == 2.0); // Should default to same value
  }
  
  SECTION("Parse scale with two values")
  {
    CSSTransformParser parser("scale(2, 3)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kScale);
    REQUIRE(functions[0].values.size() == 2);
    REQUIRE(functions[0].values[0] == 2.0);
    REQUIRE(functions[0].values[1] == 3.0);
  }
  
  SECTION("Parse scaleX")
  {
    CSSTransformParser parser("scaleX(1.5)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kScaleX);
    REQUIRE(functions[0].values.size() == 1);
    REQUIRE(functions[0].values[0] == 1.5);
  }
}

TEST_CASE("CSSTransformParser matrix functions", "[css-transform-parser]")
{
  SECTION("Parse matrix")
  {
    CSSTransformParser parser("matrix(1, 0, 0, 1, 10, 20)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kMatrix);
    REQUIRE(functions[0].values.size() == 6);
    REQUIRE(functions[0].values[0] == 1.0);
    REQUIRE(functions[0].values[1] == 0.0);
    REQUIRE(functions[0].values[2] == 0.0);
    REQUIRE(functions[0].values[3] == 1.0);
    REQUIRE(functions[0].values[4] == 10.0);
    REQUIRE(functions[0].values[5] == 20.0);
  }
  
  SECTION("Parse matrix3d")
  {
    CSSTransformParser parser("matrix3d(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 10, 20, 30, 1)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kMatrix3D);
    REQUIRE(functions[0].values.size() == 16);
    REQUIRE(functions[0].values[12] == 10.0); // Translation X
    REQUIRE(functions[0].values[13] == 20.0); // Translation Y
    REQUIRE(functions[0].values[14] == 30.0); // Translation Z
  }
}

TEST_CASE("CSSTransformParser multiple functions", "[css-transform-parser]")
{
  SECTION("Parse multiple transforms")
  {
    CSSTransformParser parser("translateX(10px) rotate(45deg) scale(2)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 3);
    
    REQUIRE(functions[0].type == TransformFunctionType::kTranslateX);
    REQUIRE(functions[0].values[0] == 10.0);
    
    REQUIRE(functions[1].type == TransformFunctionType::kRotate);
    REQUIRE(functions[1].values[0] == 45.0);
    
    REQUIRE(functions[2].type == TransformFunctionType::kScale);
    REQUIRE(functions[2].values[0] == 2.0);
  }
}

TEST_CASE("CSSTransformParser skew functions", "[css-transform-parser]")
{
  SECTION("Parse skew with two values")
  {
    CSSTransformParser parser("skew(10deg, 20deg)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kSkew);
    REQUIRE(functions[0].values.size() == 2);
    REQUIRE(functions[0].values[0] == 10.0);
    REQUIRE(functions[0].values[1] == 20.0);
  }
  
  SECTION("Parse skew with one value")
  {
    CSSTransformParser parser("skew(10deg)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kSkew);
    REQUIRE(functions[0].values.size() == 2);
    REQUIRE(functions[0].values[0] == 10.0);
    REQUIRE(functions[0].values[1] == 0.0); // Should default to 0
  }
  
  SECTION("Parse skewX")
  {
    CSSTransformParser parser("skewX(15deg)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kSkewX);
    REQUIRE(functions[0].values.size() == 1);
    REQUIRE(functions[0].values[0] == 15.0);
  }
}

TEST_CASE("CSSTransformParser negative values", "[css-transform-parser]")
{
  SECTION("Parse negative translateX")
  {
    CSSTransformParser parser("translateX(-10px)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kTranslateX);
    REQUIRE(functions[0].values.size() == 1);
    REQUIRE(functions[0].values[0] == -10.0);
    REQUIRE(functions[0].units[0] == "px");
  }
  
  SECTION("Parse negative rotate")
  {
    CSSTransformParser parser("rotate(-45deg)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kRotate);
    REQUIRE(functions[0].values.size() == 1);
    REQUIRE(functions[0].values[0] == -45.0);
    REQUIRE(functions[0].units[0] == "deg");
  }
  
  SECTION("Parse negative scale")
  {
    CSSTransformParser parser("scale(-1)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kScale);
    REQUIRE(functions[0].values.size() == 2);
    REQUIRE(functions[0].values[0] == -1.0);
    REQUIRE(functions[0].values[1] == -1.0); // Should default to same value
  }
  
  SECTION("Parse translate with negative values")
  {
    CSSTransformParser parser("translate(-10px, -20px)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kTranslate);
    REQUIRE(functions[0].values.size() == 2);
    REQUIRE(functions[0].values[0] == -10.0);
    REQUIRE(functions[0].values[1] == -20.0);
    REQUIRE(functions[0].units[0] == "px");
    REQUIRE(functions[0].units[1] == "px");
  }
  
  SECTION("Parse mixed positive and negative values")
  {
    CSSTransformParser parser("translate(10px, -20px)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kTranslate);
    REQUIRE(functions[0].values.size() == 2);
    REQUIRE(functions[0].values[0] == 10.0);
    REQUIRE(functions[0].values[1] == -20.0);
    REQUIRE(functions[0].units[0] == "px");
    REQUIRE(functions[0].units[1] == "px");
  }
  
  SECTION("Parse negative decimal values")
  {
    CSSTransformParser parser("scaleX(-0.5)");
    auto functions = parser.parse();
    
    REQUIRE(parser.isValid());
    REQUIRE(functions.size() == 1);
    REQUIRE(functions[0].type == TransformFunctionType::kScaleX);
    REQUIRE(functions[0].values.size() == 1);
    REQUIRE(functions[0].values[0] == -0.5);
  }
}

TEST_CASE("CSSTransformParser error handling", "[css-transform-parser]")
{
  SECTION("Invalid function name")
  {
    CSSTransformParser parser("invalidFunction(10px)");
    auto functions = parser.parse();
    
    REQUIRE_FALSE(parser.isValid());
    REQUIRE_FALSE(parser.getError().empty());
  }
  
  SECTION("Missing closing parenthesis")
  {
    CSSTransformParser parser("translateX(10px");
    auto functions = parser.parse();
    
    REQUIRE_FALSE(parser.isValid());
  }
  
  SECTION("Wrong number of parameters")
  {
    CSSTransformParser parser("matrix(1, 2, 3)"); // Should have 6 parameters
    auto functions = parser.parse();
    
    REQUIRE_FALSE(parser.isValid());
  }
}

TEST_CASE("Transform class integration", "[transform-integration]")
{
  SECTION("Parse simple transform")
  {
    auto transform = Parse::ParseSingleValue<Transform>("translateX(10px)");
    
    REQUIRE(transform.operations().size() == 1);
    REQUIRE(transform.operations()[0].isTranslateX());
  }
  
  SECTION("Parse multiple transforms")
  {
    auto transform = Parse::ParseSingleValue<Transform>("translateX(10px) rotate(45deg) scale(2)");
    
    REQUIRE(transform.operations().size() == 3);
    REQUIRE(transform.operations()[0].isTranslateX());
    REQUIRE(transform.operations()[1].isRotate());
    REQUIRE(transform.operations()[2].isScale());
  }
  
  SECTION("Parse none")
  {
    auto transform = Parse::ParseSingleValue<Transform>("none");
    
    REQUIRE(transform.operations().empty());
  }
  
  SECTION("Parse invalid transform should return empty")
  {
    auto transform = Parse::ParseSingleValue<Transform>("invalid(10px)");
    
    // Should return default empty transform on parsing failure
    REQUIRE(transform.operations().empty());
  }
}
