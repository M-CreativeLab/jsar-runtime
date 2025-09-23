#include "../catch2/catch_amalgamated.hpp"
#include <client/cssom/computed_style.hpp>
#include <client/cssom/values/computed/text.hpp>
#include <client/cssom/values/specified/text.hpp>

using namespace client_cssom;
using namespace client_cssom::values::computed;
using namespace client_cssom::values::specified;

TEST_CASE("CSS VerticalAlign Tests", "[css-vertical-align]")
{
  SECTION("VerticalAlign keyword parsing")
  {
    VerticalAlign baseline, sub, super, top, textTop, middle, bottom, textBottom;
    
    // Test keyword parsing
    REQUIRE(baseline.parse("baseline"));
    REQUIRE(baseline.tag() == VerticalAlign::Tag::kBaseline);
    REQUIRE(baseline.toCss() == "baseline");
    
    REQUIRE(sub.parse("sub"));
    REQUIRE(sub.tag() == VerticalAlign::Tag::kSub);
    REQUIRE(sub.toCss() == "sub");
    
    REQUIRE(super.parse("super"));
    REQUIRE(super.tag() == VerticalAlign::Tag::kSuper);
    REQUIRE(super.toCss() == "super");
    
    REQUIRE(top.parse("top"));
    REQUIRE(top.tag() == VerticalAlign::Tag::kTop);
    REQUIRE(top.toCss() == "top");
    
    REQUIRE(textTop.parse("text-top"));
    REQUIRE(textTop.tag() == VerticalAlign::Tag::kTextTop);
    REQUIRE(textTop.toCss() == "text-top");
    
    REQUIRE(middle.parse("middle"));
    REQUIRE(middle.tag() == VerticalAlign::Tag::kMiddle);
    REQUIRE(middle.toCss() == "middle");
    
    REQUIRE(bottom.parse("bottom"));
    REQUIRE(bottom.tag() == VerticalAlign::Tag::kBottom);
    REQUIRE(bottom.toCss() == "bottom");
    
    REQUIRE(textBottom.parse("text-bottom"));
    REQUIRE(textBottom.tag() == VerticalAlign::Tag::kTextBottom);
    REQUIRE(textBottom.toCss() == "text-bottom");
  }
  
  SECTION("VerticalAlign length value parsing")
  {
    VerticalAlign lengthPos, lengthNeg;
    
    // Test positive length parsing
    REQUIRE(lengthPos.parse("5px"));
    REQUIRE(lengthPos.tag() == VerticalAlign::Tag::kLength);
    REQUIRE(lengthPos.value() == 5.0f);
    REQUIRE(lengthPos.toCss() == "5.000000px");
    
    // Test negative length parsing
    REQUIRE(lengthNeg.parse("-5px"));
    REQUIRE(lengthNeg.tag() == VerticalAlign::Tag::kLength);
    REQUIRE(lengthNeg.value() == -5.0f);
    REQUIRE(lengthNeg.toCss() == "-5.000000px");
  }
  
  SECTION("VerticalAlign percentage value parsing")
  {
    VerticalAlign percentPos, percentNeg;
    
    // Test positive percentage parsing
    REQUIRE(percentPos.parse("10%"));
    REQUIRE(percentPos.tag() == VerticalAlign::Tag::kPercentage);
    REQUIRE(percentPos.value() == 10.0f);
    REQUIRE(percentPos.toCss() == "10.000000%");
    
    // Test negative percentage parsing
    REQUIRE(percentNeg.parse("-10%"));
    REQUIRE(percentNeg.tag() == VerticalAlign::Tag::kPercentage);
    REQUIRE(percentNeg.value() == -10.0f);
    REQUIRE(percentNeg.toCss() == "-10.000000%");
  }
  
  SECTION("VerticalAlign static factory methods")
  {
    auto baseline = VerticalAlign::Baseline();
    auto sub = VerticalAlign::Sub();
    auto super = VerticalAlign::Super();
    auto top = VerticalAlign::Top();
    auto textTop = VerticalAlign::TextTop();
    auto middle = VerticalAlign::Middle();
    auto bottom = VerticalAlign::Bottom();
    auto textBottom = VerticalAlign::TextBottom();
    auto length = VerticalAlign::Length(5.0f);
    auto percentage = VerticalAlign::Percentage(10.0f);
    
    REQUIRE(baseline.tag() == VerticalAlign::Tag::kBaseline);
    REQUIRE(sub.tag() == VerticalAlign::Tag::kSub);
    REQUIRE(super.tag() == VerticalAlign::Tag::kSuper);
    REQUIRE(top.tag() == VerticalAlign::Tag::kTop);
    REQUIRE(textTop.tag() == VerticalAlign::Tag::kTextTop);
    REQUIRE(middle.tag() == VerticalAlign::Tag::kMiddle);
    REQUIRE(bottom.tag() == VerticalAlign::Tag::kBottom);
    REQUIRE(textBottom.tag() == VerticalAlign::Tag::kTextBottom);
    REQUIRE(length.tag() == VerticalAlign::Tag::kLength);
    REQUIRE(length.value() == 5.0f);
    REQUIRE(percentage.tag() == VerticalAlign::Tag::kPercentage);
    REQUIRE(percentage.value() == 10.0f);
  }
  
  SECTION("Computed VerticalAlign helper methods")
  {
    computed::VerticalAlign baseline = specified::VerticalAlign::Baseline();
    computed::VerticalAlign length = specified::VerticalAlign::Length(5.0f);
    computed::VerticalAlign percentage = specified::VerticalAlign::Percentage(10.0f);
    
    // Test isBaseline
    REQUIRE(baseline.isBaseline());
    REQUIRE_FALSE(length.isBaseline());
    REQUIRE_FALSE(percentage.isBaseline());
    
    // Test isKeyword
    REQUIRE(baseline.isKeyword());
    REQUIRE_FALSE(length.isKeyword());
    REQUIRE_FALSE(percentage.isKeyword());
    
    // Test getOffset
    REQUIRE(baseline.getOffset() == 0.0f);
    REQUIRE(length.getOffset() == 5.0f);
    REQUIRE(percentage.getOffset(50.0f) == 5.0f); // 10% of 50px = 5px
  }
  
  SECTION("Invalid input handling")
  {
    VerticalAlign invalid;
    
    // Test invalid inputs
    REQUIRE_FALSE(invalid.parse("invalid"));
    REQUIRE_FALSE(invalid.parse(""));
    REQUIRE_FALSE(invalid.parse("10pt")); // Only px supported for now
    REQUIRE_FALSE(invalid.parse("invalid%"));
  }
}