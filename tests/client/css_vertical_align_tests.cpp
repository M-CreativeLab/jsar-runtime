#include "../catch2/catch_amalgamated.hpp"
#include <client/cssom/computed_style.hpp>
#include <client/cssom/values/computed/text.hpp>
#include <client/cssom/values/specified/text.hpp>

using namespace client_cssom;
using namespace client_cssom::values;

TEST_CASE("CSS VerticalAlign Tests", "[css-vertical-align]")
{
  SECTION("VerticalAlign keyword parsing")
  {
    specified::VerticalAlign baseline, sub, super, top, textTop, middle, bottom, textBottom;

    // Test keyword parsing
    baseline = Parse::ParseSingleValue<specified::VerticalAlign>("baseline");
    REQUIRE(baseline.toCss() == "baseline");

    sub = Parse::ParseSingleValue<specified::VerticalAlign>("sub");
    REQUIRE(sub.toCss() == "sub");

    super = Parse::ParseSingleValue<specified::VerticalAlign>("super");
    REQUIRE(super.toCss() == "super");

    top = Parse::ParseSingleValue<specified::VerticalAlign>("top");
    REQUIRE(top.toCss() == "top");

    textTop = Parse::ParseSingleValue<specified::VerticalAlign>("text-top");
    REQUIRE(textTop.toCss() == "text-top");

    middle = Parse::ParseSingleValue<specified::VerticalAlign>("middle");
    REQUIRE(middle.toCss() == "middle");

    bottom = Parse::ParseSingleValue<specified::VerticalAlign>("bottom");
    REQUIRE(bottom.toCss() == "bottom");

    textBottom = Parse::ParseSingleValue<specified::VerticalAlign>("text-bottom");
    REQUIRE(textBottom.toCss() == "text-bottom");
  }

  SECTION("VerticalAlign length value parsing")
  {
    specified::VerticalAlign lengthPos, lengthNeg;

    // Test positive length parsing
    lengthPos = Parse::ParseSingleValue<specified::VerticalAlign>("5px");
    REQUIRE(lengthPos.value() == 5.0f);
    REQUIRE(lengthPos.toCss() == "5.000000px");

    // Test negative length parsing
    lengthNeg = Parse::ParseSingleValue<specified::VerticalAlign>("-5px");
    REQUIRE(lengthNeg.value() == -5.0f);
    REQUIRE(lengthNeg.toCss() == "-5.000000px");
  }

  SECTION("VerticalAlign percentage value parsing")
  {
    specified::VerticalAlign percentPos, percentNeg;

    // Test positive percentage parsing
    percentPos = Parse::ParseSingleValue<specified::VerticalAlign>("10%");
    REQUIRE(percentPos.tag() == specified::VerticalAlign::kPercentage);
    REQUIRE(percentPos.value() == 10.0f);
    REQUIRE(percentPos.toCss() == "10.000000%");

    // Test negative percentage parsing
    percentNeg = Parse::ParseSingleValue<specified::VerticalAlign>("-10%");
    REQUIRE(percentNeg.tag() == specified::VerticalAlign::kPercentage);
    REQUIRE(percentNeg.value() == -10.0f);
    REQUIRE(percentNeg.toCss() == "-10.000000%");
  }

  SECTION("VerticalAlign static factory methods")
  {
    auto baseline = specified::VerticalAlign::Baseline();
    auto sub = specified::VerticalAlign::Sub();
    auto super = specified::VerticalAlign::Super();
    auto top = specified::VerticalAlign::Top();
    auto textTop = specified::VerticalAlign::TextTop();
    auto middle = specified::VerticalAlign::Middle();
    auto bottom = specified::VerticalAlign::Bottom();
    auto textBottom = specified::VerticalAlign::TextBottom();
    auto length = specified::VerticalAlign::Length(5.0f);
    auto percentage = specified::VerticalAlign::Percentage(10.0f);

    REQUIRE(baseline.tag() == specified::VerticalAlign::kBaseline);
    REQUIRE(sub.tag() == specified::VerticalAlign::kSub);
    REQUIRE(super.tag() == specified::VerticalAlign::kSuper);
    REQUIRE(top.tag() == specified::VerticalAlign::kTop);
    REQUIRE(textTop.tag() == specified::VerticalAlign::kTextTop);
    REQUIRE(middle.tag() == specified::VerticalAlign::kMiddle);
    REQUIRE(bottom.tag() == specified::VerticalAlign::kBottom);
    REQUIRE(textBottom.tag() == specified::VerticalAlign::kTextBottom);
    REQUIRE(length.tag() == specified::VerticalAlign::kLength);
    REQUIRE(length.value() == 5.0f);
    REQUIRE(percentage.tag() == specified::VerticalAlign::kPercentage);
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
}
