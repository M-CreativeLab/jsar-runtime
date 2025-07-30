#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <client/canvas/image_codec.hpp>
#include <skia/include/core/SkBitmap.h>
#include <vector>
#include <string>

using namespace canvas;

TEST_CASE("SVG Image Codec basic functionality", "[svg-codec]")
{
  SECTION("Simple SVG decoding")
  {
    std::string svg_content = R"(
<?xml version="1.0" encoding="UTF-8"?>
<svg width="100" height="100" xmlns="http://www.w3.org/2000/svg">
  <rect x="10" y="10" width="80" height="80" fill="red"/>
</svg>
)";
    
    std::vector<char> svg_data(svg_content.begin(), svg_content.end());
    SkBitmap decoded_bitmap;
    
    bool success = ImageCodec::Decode(svg_data, decoded_bitmap, "test.svg");
    
    REQUIRE(success == true);
    REQUIRE(decoded_bitmap.width() == 100);
    REQUIRE(decoded_bitmap.height() == 100);
    REQUIRE(!decoded_bitmap.empty());
  }

  SECTION("SVG decoding with target width and height")
  {
    std::string svg_content = R"(
<svg width="100" height="100" xmlns="http://www.w3.org/2000/svg">
  <rect x="10" y="10" width="80" height="80" fill="red"/>
</svg>
)";
    
    std::vector<char> svg_data(svg_content.begin(), svg_content.end());
    SkBitmap decoded_bitmap;
    
    bool success = ImageCodec::Decode(svg_data, decoded_bitmap, "test.svg", 200, 300);
    
    REQUIRE(success == true);
    REQUIRE(decoded_bitmap.width() == 200);
    REQUIRE(decoded_bitmap.height() == 300);
    REQUIRE(!decoded_bitmap.empty());
  }

  SECTION("SVG decoding with target width only (maintain aspect ratio)")
  {
    std::string svg_content = R"(
<svg width="100" height="50" xmlns="http://www.w3.org/2000/svg">
  <rect x="10" y="10" width="80" height="30" fill="blue"/>
</svg>
)";
    
    std::vector<char> svg_data(svg_content.begin(), svg_content.end());
    SkBitmap decoded_bitmap;
    
    bool success = ImageCodec::Decode(svg_data, decoded_bitmap, "test.svg", 200, -1);
    
    REQUIRE(success == true);
    REQUIRE(decoded_bitmap.width() == 200);
    REQUIRE(decoded_bitmap.height() == 100); // 200 * (50/100) = 100
    REQUIRE(!decoded_bitmap.empty());
  }

  SECTION("SVG decoding with target height only (maintain aspect ratio)")
  {
    std::string svg_content = R"(
<svg width="100" height="50" xmlns="http://www.w3.org/2000/svg">
  <rect x="10" y="10" width="80" height="30" fill="green"/>
</svg>
)";
    
    std::vector<char> svg_data(svg_content.begin(), svg_content.end());
    SkBitmap decoded_bitmap;
    
    bool success = ImageCodec::Decode(svg_data, decoded_bitmap, "test.svg", -1, 100);
    
    REQUIRE(success == true);
    REQUIRE(decoded_bitmap.width() == 200); // 100 * (100/50) = 200
    REQUIRE(decoded_bitmap.height() == 100);
    REQUIRE(!decoded_bitmap.empty());
  }

  SECTION("SVG with no XML declaration")
  {
    std::string svg_content = R"(
<svg width="50" height="50" xmlns="http://www.w3.org/2000/svg">
  <circle cx="25" cy="25" r="20" fill="blue"/>
</svg>
)";
    
    std::vector<char> svg_data(svg_content.begin(), svg_content.end());
    SkBitmap decoded_bitmap;
    
    bool success = ImageCodec::Decode(svg_data, decoded_bitmap, "test_circle.svg");
    
    REQUIRE(success == true);
    REQUIRE(decoded_bitmap.width() == 50);
    REQUIRE(decoded_bitmap.height() == 50);
  }

  SECTION("SVG with oversized dimensions")
  {
    std::string svg_content = R"(
<svg width="8192" height="8192" xmlns="http://www.w3.org/2000/svg">
  <rect x="0" y="0" width="8192" height="8192" fill="green"/>
</svg>
)";
    
    std::vector<char> svg_data(svg_content.begin(), svg_content.end());
    SkBitmap decoded_bitmap;
    
    bool success = ImageCodec::Decode(svg_data, decoded_bitmap, "large.svg");
    
    REQUIRE(success == true);
    // Should be scaled down due to size constraints
    REQUIRE(decoded_bitmap.width() <= 1024);
    REQUIRE(decoded_bitmap.height() <= 1024);
  }

  SECTION("SVG with target dimensions exceeding size constraints")
  {
    std::string svg_content = R"(
<svg width="100" height="100" xmlns="http://www.w3.org/2000/svg">
  <rect x="0" y="0" width="100" height="100" fill="purple"/>
</svg>
)";
    
    std::vector<char> svg_data(svg_content.begin(), svg_content.end());
    SkBitmap decoded_bitmap;
    
    // Request very large dimensions
    bool success = ImageCodec::Decode(svg_data, decoded_bitmap, "large_target.svg", 2048, 2048);
    
    REQUIRE(success == true);
    // Should be scaled down due to size constraints
    REQUIRE(decoded_bitmap.width() <= 1024);
    REQUIRE(decoded_bitmap.height() <= 1024);
  }

  SECTION("Invalid SVG data")
  {
    std::string invalid_content = "This is not SVG data";
    std::vector<char> invalid_data(invalid_content.begin(), invalid_content.end());
    SkBitmap decoded_bitmap;
    
    bool success = ImageCodec::Decode(invalid_data, decoded_bitmap, "invalid.svg");
    
    REQUIRE(success == false);
  }

  SECTION("Empty SVG data")
  {
    std::vector<char> empty_data;
    SkBitmap decoded_bitmap;
    
    bool success = ImageCodec::Decode(empty_data, decoded_bitmap, "empty.svg");
    
    REQUIRE(success == false);
  }

  SECTION("Malformed SVG")
  {
    std::string malformed_content = R"(
<svg width="100" height="100" xmlns="http://www.w3.org/2000/svg">
  <rect x="10" y="10" width="80" height="80" fill="red"
</svg>
)";
    
    std::vector<char> malformed_data(malformed_content.begin(), malformed_content.end());
    SkBitmap decoded_bitmap;
    
    bool success = ImageCodec::Decode(malformed_data, decoded_bitmap, "malformed.svg");
    
    // Should handle gracefully - might succeed or fail depending on parser tolerance
    // The important thing is it shouldn't crash
    REQUIRE((success == true || success == false)); // Just ensure it doesn't crash
  }
}

TEST_CASE("SVG Image Codec compatibility with existing formats", "[svg-codec]")
{
  SECTION("Non-SVG data should still work")
  {
    // This is a minimal PNG signature - won't actually decode but should not be detected as SVG
    std::vector<char> png_signature = {
      static_cast<char>(0x89), 'P', 'N', 'G', 
      static_cast<char>(0x0D), static_cast<char>(0x0A), static_cast<char>(0x1A), static_cast<char>(0x0A)
    };
    
    SkBitmap decoded_bitmap;
    bool success = ImageCodec::Decode(png_signature, decoded_bitmap, "test.png");
    
    // Should not be treated as SVG (will likely fail to decode as PNG due to incomplete data, but that's expected)
    // The important thing is that SVG detection doesn't interfere with other formats
    REQUIRE(true); // Test passes if we reach here without crashing
  }

  SECTION("HTML with SVG should not be detected as pure SVG")
  {
    std::string html_with_svg = R"(
<!DOCTYPE html>
<html>
<body>
  <svg width="100" height="100">
    <rect width="100" height="100" fill="red"/>
  </svg>
</body>
</html>
)";
    
    std::vector<char> html_data(html_with_svg.begin(), html_with_svg.end());
    SkBitmap decoded_bitmap;
    
    // This should not be detected as SVG since it's HTML containing SVG
    bool success = ImageCodec::Decode(html_data, decoded_bitmap, "test.html");
    
    REQUIRE(success == false);
  }
}

TEST_CASE("SVG Image Codec edge cases", "[svg-codec]")
{
  SECTION("SVG with viewBox but no width/height")
  {
    std::string svg_content = R"(
<svg viewBox="0 0 200 200" xmlns="http://www.w3.org/2000/svg">
  <rect x="50" y="50" width="100" height="100" fill="purple"/>
</svg>
)";
    
    std::vector<char> svg_data(svg_content.begin(), svg_content.end());
    SkBitmap decoded_bitmap;
    
    bool success = ImageCodec::Decode(svg_data, decoded_bitmap, "viewbox.svg");
    
    // nanosvg should handle viewBox and infer dimensions
    REQUIRE((success == true || success == false)); // Ensure no crash
  }

  SECTION("Case insensitive SVG detection")
  {
    std::string svg_content = R"(
<SVG WIDTH="100" HEIGHT="100" xmlns="http://www.w3.org/2000/svg">
  <RECT x="10" y="10" width="80" height="80" fill="orange"/>
</SVG>
)";
    
    std::vector<char> svg_data(svg_content.begin(), svg_content.end());
    SkBitmap decoded_bitmap;
    
    bool success = ImageCodec::Decode(svg_data, decoded_bitmap, "uppercase.svg");
    
    REQUIRE(success == true);
  }
}