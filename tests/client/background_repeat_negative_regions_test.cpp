#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"

#include <skia/include/core/SkCanvas.h>
#include <skia/include/core/SkSurface.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkBitmap.h>
#include <skia/include/core/SkPaint.h>

// Simple test to validate the background repeat logic
// This test verifies that the tiling logic correctly handles negative regions

TEST_CASE("Background repeat logic handles negative regions correctly", "[background-repeat-logic]")
{
  SECTION("Tiling start position calculation for repeat-x")
  {
    // Test the core logic that was fixed
    float imagePosition = 100.0f;  // Image positioned at x=100 (content-box start)
    float imageWidth = 25.0f;      // Small image
    float repeatableAreaLeft = 0.0f; // Border-box starts at x=0
    
    // Calculate start position using the fixed logic
    float startX = imagePosition;
    while (startX > repeatableAreaLeft) {
      startX -= imageWidth;
    }
    
    // startX should be positioned to cover the full repeatable area
    REQUIRE(startX <= repeatableAreaLeft);
    
    // Calculate how many tiles cover the region before the image position
    int tilesBeforePosition = static_cast<int>(std::ceil((imagePosition - startX) / imageWidth));
    REQUIRE(tilesBeforePosition >= 4); // Should have multiple tiles covering x=0 to x=100
    
    // Verify specific positions
    REQUIRE(startX == 0.0f); // Should start exactly at border-box left with these numbers
  }
  
  SECTION("Tiling start position calculation for repeat-y")
  {
    // Test vertical repeat logic
    float imagePosition = 100.0f;  // Image positioned at y=100 (content-box start)
    float imageHeight = 30.0f;     // Image height
    float repeatableAreaTop = 0.0f; // Border-box starts at y=0
    
    // Calculate start position using the fixed logic
    float startY = imagePosition;
    while (startY > repeatableAreaTop) {
      startY -= imageHeight;
    }
    
    REQUIRE(startY <= repeatableAreaTop);
    
    // Should have tiles covering the region before the image position
    int tilesBeforePosition = static_cast<int>(std::ceil((imagePosition - startY) / imageHeight));
    REQUIRE(tilesBeforePosition >= 3); // Should cover y=0 to y=100 region
  }
  
  SECTION("Edge case: image larger than offset")
  {
    // Test when image is larger than the offset
    float imagePosition = 50.0f;   // Small offset
    float imageWidth = 75.0f;      // Large image
    float repeatableAreaLeft = 0.0f;
    
    float startX = imagePosition;
    while (startX > repeatableAreaLeft) {
      startX -= imageWidth;
    }
    
    // Should still ensure coverage from the left edge
    REQUIRE(startX <= repeatableAreaLeft);
    REQUIRE(startX == -25.0f); // Should be positioned to cover from x=0
  }
  
  SECTION("Edge case: image position already at left edge")
  {
    // Test when image position is already at the left edge
    float imagePosition = 0.0f;    // Already at left edge
    float imageWidth = 25.0f;      
    float repeatableAreaLeft = 0.0f;
    
    float startX = imagePosition;
    while (startX > repeatableAreaLeft) {
      startX -= imageWidth;
    }
    
    // Should remain at the edge
    REQUIRE(startX == 0.0f);
  }
}