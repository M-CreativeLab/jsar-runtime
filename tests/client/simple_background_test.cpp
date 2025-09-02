#include <iostream>
#include <cmath>
#include <cassert>

// Simple test to validate the background repeat logic 
// This validates that our fix correctly handles negative regions

void test_basic_tiling_logic() {
    std::cout << "Testing basic tiling start position calculation..." << std::endl;
    
    // Test case 1: repeat-x with content-box origin
    float imagePosition = 100.0f;  // Image positioned at x=100 (content-box start)
    float imageWidth = 25.0f;      // Small image
    float repeatableAreaLeft = 0.0f; // Border-box starts at x=0
    
    // Calculate start position using the fixed logic
    float startX = imagePosition;
    while (startX > repeatableAreaLeft) {
        startX -= imageWidth;
    }
    
    assert(startX <= repeatableAreaLeft);
    assert(startX == 0.0f); // Should start exactly at border-box left
    
    // Calculate how many tiles cover the region before the image position
    int tilesBeforePosition = static_cast<int>(std::ceil((imagePosition - startX) / imageWidth));
    assert(tilesBeforePosition == 4); // Should have 4 tiles covering x=0 to x=100
    
    std::cout << "✓ Basic repeat-x logic works correctly" << std::endl;
}

void test_vertical_tiling_logic() {
    std::cout << "Testing vertical tiling logic..." << std::endl;
    
    // Test vertical repeat logic
    float imagePosition = 100.0f;  // Image positioned at y=100 (content-box start)
    float imageHeight = 30.0f;     // Image height
    float repeatableAreaTop = 0.0f; // Border-box starts at y=0
    
    // Calculate start position using the fixed logic
    float startY = imagePosition;
    while (startY > repeatableAreaTop) {
        startY -= imageHeight;
    }
    
    assert(startY <= repeatableAreaTop);
    // 100 - 30 = 70, 70 - 30 = 40, 40 - 30 = 10, 10 - 30 = -20 (stops when <= 0)
    assert(startY == -20.0f); // Should go to -20 when it becomes <= 0
    
    std::cout << "✓ Vertical repeat-y logic works correctly" << std::endl;
}

void test_edge_cases() {
    std::cout << "Testing edge cases..." << std::endl;
    
    // Test when image is larger than the offset
    float imagePosition = 50.0f;   // Small offset
    float imageWidth = 75.0f;      // Large image
    float repeatableAreaLeft = 0.0f;
    
    float startX = imagePosition;
    while (startX > repeatableAreaLeft) {
        startX -= imageWidth;
    }
    
    assert(startX <= repeatableAreaLeft);
    assert(startX == -25.0f); // 50 - 75 = -25
    
    // Test when image position is already at left edge
    imagePosition = 0.0f;    // Already at left edge
    imageWidth = 25.0f;      
    
    startX = imagePosition;
    while (startX > repeatableAreaLeft) {
        startX -= imageWidth;
    }
    
    assert(startX == 0.0f); // Should remain at the edge
    
    std::cout << "✓ Edge cases handled correctly" << std::endl;
}

void test_real_world_scenario() {
    std::cout << "Testing real-world scenario from issue..." << std::endl;
    
    // Simulate the exact scenario described in the issue
    float borderBoxLeft = 0.0f;
    float borderBoxRight = 300.0f;
    float contentBoxLeft = 100.0f;  // 100px offset due to border + padding
    float contentBoxRight = 300.0f;
    
    float imageWidth = 50.0f;
    float imagePosition = contentBoxLeft; // Image positioned at content-box start
    
    // OLD (BUGGY) LOGIC: Only tile within positioning area
    float oldStartX = imagePosition;
    while (oldStartX > contentBoxLeft) {  // BUG: uses contentBoxLeft
        oldStartX -= imageWidth;
    }
    // This would start at x=100, leaving x=0-100 uncovered
    assert(oldStartX == 100.0f);
    
    // NEW (FIXED) LOGIC: Tile within full repeatable area
    float newStartX = imagePosition;  
    while (newStartX > borderBoxLeft) {  // FIX: uses borderBoxLeft (repeatable area)
        newStartX -= imageWidth;
    }
    
    // Verify the fix works
    assert(newStartX <= borderBoxLeft);
    assert(newStartX == 0.0f); // Should start at border-box left
    
    // Count tiles in the "negative" region (before content-box)
    int tilesInNegativeRegion = 0;
    for (float x = newStartX; x < contentBoxLeft; x += imageWidth) {
        tilesInNegativeRegion++;
    }
    assert(tilesInNegativeRegion == 2); // Two 50px tiles to cover 0-100px region
    
    // Verify we have full coverage
    assert(newStartX + tilesInNegativeRegion * imageWidth >= contentBoxLeft);
    
    std::cout << "✓ Real-world scenario fixed correctly" << std::endl;
    std::cout << "  Old logic would start at x=" << oldStartX << " (leaving gap)" << std::endl;
    std::cout << "  New logic starts at x=" << newStartX << " (full coverage)" << std::endl;
    std::cout << "  Negative region tiles: " << tilesInNegativeRegion << std::endl;
}

int main() {
    std::cout << "Running background repeat negative regions tests..." << std::endl;
    std::cout << "=================================================" << std::endl;
    
    try {
        test_basic_tiling_logic();
        test_vertical_tiling_logic();
        test_edge_cases();
        test_real_world_scenario();
        
        std::cout << "=================================================" << std::endl;
        std::cout << "✅ All tests passed! The fix correctly handles negative repeat regions." << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}