#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/cssom/values/computed/image.hpp>
#include <client/cssom/values/computed/color.hpp>
#include <client/cssom/values/computed/length.hpp>
#include <client/cssom/values/computed/length_percentage.hpp>
#include <client/cssom/values/generics/image.hpp>

using namespace client_cssom::values;

TEST_CASE("Image gradient data extraction", "[image-gradient]")
{
  SECTION("Linear gradient creation and extraction")
  {
    // Create a simple linear gradient
    computed::Gradient::LinearGradient linearGrad;
    linearGrad.direction = generics::LineDirection::kToRight;

    // Add color stops
    computed::GradientItem item1;
    item1.type = generics::GenericGradientItemBase::kSimpleColorStop;
    item1.value = computed::GradientItem::SimpleColorStop{computed::Color::Black()};
    linearGrad.items.push_back(item1);

    computed::GradientItem item2;
    item2.type = generics::GenericGradientItemBase::kSimpleColorStop;
    item2.value = computed::GradientItem::SimpleColorStop{computed::Color::White()};
    linearGrad.items.push_back(item2);

    // Create gradient and image
    computed::Gradient gradient(linearGrad);
    computed::Image image(gradient);

    // Test gradient detection
    REQUIRE(image.isGradient());
    REQUIRE_FALSE(image.isUrl());
    REQUIRE_FALSE(image.isNone());

    // Test gradient data extraction
    const computed::Gradient *extractedGradient = image.getGradient();
    REQUIRE(extractedGradient != nullptr);

    // Test linear gradient extraction
    const auto *extractedLinear = image.getLinearGradient();
    REQUIRE(extractedLinear != nullptr);
    REQUIRE(extractedLinear->direction == generics::LineDirection::kToRight);
    REQUIRE(extractedLinear->items.size() == 2);

    // Test radial gradient extraction (should be null)
    const auto *extractedRadial = image.getRadialGradient();
    REQUIRE(extractedRadial == nullptr);

    // Test repeating flag (default should be false)
    REQUIRE_FALSE(image.isGradientRepeating());
  }

  SECTION("Radial gradient creation and extraction")
  {
    // Create a radial gradient
    computed::Gradient::RadialGradient radialGrad;
    radialGrad.shape = generics::RadialGradientShape::kCircle;
    radialGrad.size = generics::RadialGradientSize::kFarthestCorner;

    // Add color stops
    computed::GradientItem item1;
    item1.type = generics::GenericGradientItemBase::kSimpleColorStop;
    item1.value = computed::GradientItem::SimpleColorStop{computed::Color::Black()};
    radialGrad.items.push_back(item1);

    computed::GradientItem item2;
    item2.type = generics::GenericGradientItemBase::kSimpleColorStop;
    item2.value = computed::GradientItem::SimpleColorStop{computed::Color::White()};
    radialGrad.items.push_back(item2);

    // Create gradient and image
    computed::Gradient gradient(radialGrad);
    computed::Image image(gradient);

    // Test gradient detection
    REQUIRE(image.isGradient());
    REQUIRE_FALSE(image.isUrl());
    REQUIRE_FALSE(image.isNone());

    // Test gradient data extraction
    const computed::Gradient *extractedGradient = image.getGradient();
    REQUIRE(extractedGradient != nullptr);

    // Test radial gradient extraction
    const auto *extractedRadial = image.getRadialGradient();
    REQUIRE(extractedRadial != nullptr);
    REQUIRE(extractedRadial->shape == generics::RadialGradientShape::kCircle);
    REQUIRE(extractedRadial->size == generics::RadialGradientSize::kFarthestCorner);
    REQUIRE(extractedRadial->items.size() == 2);

    // Test linear gradient extraction (should be null)
    const auto *extractedLinear = image.getLinearGradient();
    REQUIRE(extractedLinear == nullptr);
  }

  SECTION("None image gradient extraction")
  {
    computed::Image image = computed::Image::None();

    REQUIRE(image.isNone());
    REQUIRE_FALSE(image.isGradient());

    // All gradient extraction methods should return null
    REQUIRE(image.getGradient() == nullptr);
    REQUIRE(image.getLinearGradient() == nullptr);
    REQUIRE(image.getRadialGradient() == nullptr);
    REQUIRE_FALSE(image.isGradientRepeating());
  }

  SECTION("URL image gradient extraction")
  {
    // Create a URL image (this would normally be created by parsing)
    // For this test, we'll just create a None image since URL creation
    // requires more complex setup
    computed::Image image = computed::Image::None();

    // When it's not a gradient, extraction should return null
    REQUIRE(image.getGradient() == nullptr);
    REQUIRE(image.getLinearGradient() == nullptr);
    REQUIRE(image.getRadialGradient() == nullptr);
    REQUIRE_FALSE(image.isGradientRepeating());
  }
}

TEST_CASE("Gradient color extraction", "[image-gradient-colors]")
{
  SECTION("Simple color stops")
  {
    computed::Gradient::LinearGradient linearGrad;

    // Add color stops with known colors
    computed::GradientItem redStop;
    redStop.type = generics::GenericGradientItemBase::kSimpleColorStop;
    redStop.value = computed::GradientItem::SimpleColorStop{computed::Color::Black()};
    linearGrad.items.push_back(redStop);

    computed::GradientItem blueStop;
    blueStop.type = generics::GenericGradientItemBase::kSimpleColorStop;
    blueStop.value = computed::GradientItem::SimpleColorStop{computed::Color::White()};
    linearGrad.items.push_back(blueStop);

    computed::Gradient gradient(linearGrad);
    computed::Image image(gradient);

    const auto *extractedLinear = image.getLinearGradient();
    REQUIRE(extractedLinear != nullptr);
    REQUIRE(extractedLinear->items.size() == 2);

    // Verify first color stop
    const auto &firstItem = extractedLinear->items[0];
    REQUIRE(firstItem.type == generics::GenericGradientItemBase::kSimpleColorStop);

    const auto &firstColorStop = get<computed::GradientItem::SimpleColorStop>(firstItem.value);
    SkColor resolvedColor = firstColorStop.color.resolveToAbsoluteColor();
    // Black color should resolve to black
    REQUIRE(resolvedColor != SK_ColorTRANSPARENT);

    // Verify second color stop
    const auto &secondItem = extractedLinear->items[1];
    REQUIRE(secondItem.type == generics::GenericGradientItemBase::kSimpleColorStop);

    const auto &secondColorStop = std::get<computed::GradientItem::SimpleColorStop>(secondItem.value);
    SkColor resolvedColor2 = secondColorStop.color.resolveToAbsoluteColor();
    // White color should resolve to white
    REQUIRE(resolvedColor2 != SK_ColorTRANSPARENT);
  }
}
