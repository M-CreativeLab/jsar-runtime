#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/cssom/values/computed/image.hpp>
#include <client/cssom/values/computed/color.hpp>
#include <client/cssom/values/computed/length.hpp>
#include <client/cssom/values/computed/length_percentage.hpp>
#include <client/cssom/values/generics/image.hpp>

using namespace client_cssom::values::computed;
using namespace client_cssom::values::generics;

TEST_CASE("Image gradient data extraction", "[image-gradient]")
{
  SECTION("Linear gradient creation and extraction")
  {
    // Create a simple linear gradient
    typename Gradient::LinearGradient linearGrad;
    linearGrad.direction = LineDirection::kToRight;
    
    // Add color stops
    GenericGradientItem<Color, LengthPercentage> item1;
    item1.type = GenericGradientItem<Color, LengthPercentage>::kSimpleColorStop;
    item1.value = typename GenericGradientItem<Color, LengthPercentage>::SimpleColorStop{Color::Black()};
    linearGrad.items.push_back(item1);
    
    GenericGradientItem<Color, LengthPercentage> item2;
    item2.type = GenericGradientItem<Color, LengthPercentage>::kSimpleColorStop;
    item2.value = typename GenericGradientItem<Color, LengthPercentage>::SimpleColorStop{Color::White()};
    linearGrad.items.push_back(item2);
    
    // Create gradient and image
    Gradient gradient(linearGrad);
    Image image(gradient);
    
    // Test gradient detection
    REQUIRE(image.isGradient());
    REQUIRE_FALSE(image.isUrl());
    REQUIRE_FALSE(image.isNone());
    
    // Test gradient data extraction
    const Gradient* extractedGradient = image.getGradient();
    REQUIRE(extractedGradient != nullptr);
    
    // Test linear gradient extraction
    const auto* extractedLinear = image.getLinearGradient();
    REQUIRE(extractedLinear != nullptr);
    REQUIRE(extractedLinear->direction == LineDirection::kToRight);
    REQUIRE(extractedLinear->items.size() == 2);
    
    // Test radial gradient extraction (should be null)
    const auto* extractedRadial = image.getRadialGradient();
    REQUIRE(extractedRadial == nullptr);
    
    // Test repeating flag (default should be false)
    REQUIRE_FALSE(image.isGradientRepeating());
  }
  
  SECTION("Radial gradient creation and extraction")
  {
    // Create a radial gradient
    typename Gradient::RadialGradient radialGrad;
    radialGrad.shape = Gradient::RadialGradient::kCircle;
    radialGrad.size = Gradient::RadialGradient::kFarthestCorner;
    
    // Add color stops
    GenericGradientItem<Color, LengthPercentage> item1;
    item1.type = GenericGradientItem<Color, LengthPercentage>::kSimpleColorStop;
    item1.value = typename GenericGradientItem<Color, LengthPercentage>::SimpleColorStop{Color::Black()};
    radialGrad.items.push_back(item1);
    
    GenericGradientItem<Color, LengthPercentage> item2;
    item2.type = GenericGradientItem<Color, LengthPercentage>::kSimpleColorStop;
    item2.value = typename GenericGradientItem<Color, LengthPercentage>::SimpleColorStop{Color::White()};
    radialGrad.items.push_back(item2);
    
    // Create gradient and image
    Gradient gradient(radialGrad);
    Image image(gradient);
    
    // Test gradient detection
    REQUIRE(image.isGradient());
    REQUIRE_FALSE(image.isUrl());
    REQUIRE_FALSE(image.isNone());
    
    // Test gradient data extraction
    const Gradient* extractedGradient = image.getGradient();
    REQUIRE(extractedGradient != nullptr);
    
    // Test radial gradient extraction
    const auto* extractedRadial = image.getRadialGradient();
    REQUIRE(extractedRadial != nullptr);
    REQUIRE(extractedRadial->shape == Gradient::RadialGradient::kCircle);
    REQUIRE(extractedRadial->size == Gradient::RadialGradient::kFarthestCorner);
    REQUIRE(extractedRadial->items.size() == 2);
    
    // Test linear gradient extraction (should be null)
    const auto* extractedLinear = image.getLinearGradient();
    REQUIRE(extractedLinear == nullptr);
  }
  
  SECTION("Repeating gradient")
  {
    // Create a repeating linear gradient
    typename Gradient::LinearGradient linearGrad;
    linearGrad.direction = LineDirection::kToBottom;
    
    Gradient gradient(linearGrad);
    gradient.repeating = true;
    
    Image image(gradient);
    
    // Test repeating flag
    REQUIRE(image.isGradientRepeating());
    
    const auto* extractedLinear = image.getLinearGradient();
    REQUIRE(extractedLinear != nullptr);
    REQUIRE(extractedLinear->direction == LineDirection::kToBottom);
  }
  
  SECTION("None image gradient extraction")
  {
    Image image = Image::None();
    
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
    Image image = Image::None();
    
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
    typename Gradient::LinearGradient linearGrad;
    
    // Add color stops with known colors
    GenericGradientItem<Color, LengthPercentage> redStop;
    redStop.type = GenericGradientItem<Color, LengthPercentage>::kSimpleColorStop;
    redStop.value = typename GenericGradientItem<Color, LengthPercentage>::SimpleColorStop{Color::Black()};
    linearGrad.items.push_back(redStop);
    
    GenericGradientItem<Color, LengthPercentage> blueStop;
    blueStop.type = GenericGradientItem<Color, LengthPercentage>::kSimpleColorStop;
    blueStop.value = typename GenericGradientItem<Color, LengthPercentage>::SimpleColorStop{Color::White()};
    linearGrad.items.push_back(blueStop);
    
    Gradient gradient(linearGrad);
    Image image(gradient);
    
    const auto* extractedLinear = image.getLinearGradient();
    REQUIRE(extractedLinear != nullptr);
    REQUIRE(extractedLinear->items.size() == 2);
    
    // Verify first color stop
    const auto& firstItem = extractedLinear->items[0];
    REQUIRE(firstItem.type == GenericGradientItem<Color, LengthPercentage>::kSimpleColorStop);
    
    const auto& firstColorStop = std::get<typename GenericGradientItem<Color, LengthPercentage>::SimpleColorStop>(firstItem.value);
    SkColor resolvedColor = firstColorStop.color.resolveToAbsoluteColor();
    // Black color should resolve to black
    REQUIRE(resolvedColor != SK_ColorTRANSPARENT);
    
    // Verify second color stop
    const auto& secondItem = extractedLinear->items[1];
    REQUIRE(secondItem.type == GenericGradientItem<Color, LengthPercentage>::kSimpleColorStop);
    
    const auto& secondColorStop = std::get<typename GenericGradientItem<Color, LengthPercentage>::SimpleColorStop>(secondItem.value);
    SkColor resolvedColor2 = secondColorStop.color.resolveToAbsoluteColor();
    // White color should resolve to white
    REQUIRE(resolvedColor2 != SK_ColorTRANSPARENT);
  }
}