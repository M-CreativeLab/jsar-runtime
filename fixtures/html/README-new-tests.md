# HTML Test Fixtures - New Additions

This document describes the three new HTML test fixtures added to test specific rendering behaviors in JSAR Runtime.

## Test Files Overview

### 1. img-edge-truncation-test.html
**Purpose**: Test image rendering edge truncation behavior
**Lines**: 447 | **Test Items**: 10

Tests whether `<img>` elements are properly truncated at container boundaries without rendering artifacts.

**Key Test Areas**:
- Basic overflow with different container sizes (100x100, 200x150, 300x200)
- Object-fit properties (cover, contain, fill, scale-down)
- Absolutely positioned images extending outside containers
- Rounded container clipping (border-radius)
- Edge cases: zero-sized containers, very small containers
- Performance test with 50 clipped images

**Critical Checks**:
- Images should be cleanly cut at container boundaries
- No bleeding or edge artifacts
- SVG and raster images should behave consistently
- Rounded corners should clip properly

### 2. text-vertical-alignment-test.html
**Purpose**: Test text vertical alignment and positioning
**Lines**: 674 | **Test Items**: 17

Addresses the specific issue where JSAR text rendering appears to sink too low compared to other browsers.

**Key Test Areas**:
- Basic vertical-align values (baseline, top, middle, bottom, super, sub)
- Mixed font sizes with consistent baseline alignment
- CJK (Chinese/Japanese/Korean) vs Latin character alignment comparison
- Inline-block element alignment
- Flexbox alignment (align-items: flex-start, center, end, baseline)
- Table cell vertical alignment
- Complex nested alignment scenarios

**Critical Checks**:
- Chinese and Latin characters should align to same baseline
- Mixed font sizes should maintain baseline consistency
- Vertical guides help identify alignment issues
- Interactive debugging features with visual markers

### 3. text-line-height-test.html
**Purpose**: Comprehensive line-height property testing
**Lines**: 802 | **Test Items**: 10

Tests all variations of line-height property to ensure correct spacing calculations.

**Key Test Areas**:
- Basic unitless values (normal, 1, 1.2, 1.5, 2, 3)
- Pixel values (12px, 16px, 24px, 40px, 50px, 60px)
- Percentage values (100%, 120%, 150%, 200%)
- Em values (1em, 1.5em, 2em)
- Inheritance patterns (unitless vs fixed values)
- Interactive demonstration with real-time adjustment
- Edge cases (very small/large values, zero line-height)
- Performance stress test with 100+ lines

**Critical Checks**:
- Different units should calculate correctly
- Inheritance should work properly (unitless values inherit multiplication factor)
- Interactive demo shows real-time computed values
- Visual debugging with background line guides

## How to Use These Tests

### For Manual Testing
1. Open any of the HTML files in a browser
2. Compare rendering with other browsers (Chrome, Firefox, Safari)
3. Look for visual differences in alignment, spacing, or clipping
4. Use browser developer tools to inspect computed values

### For Automated Testing
The files include JavaScript console logging for debugging:
- Image load/error events
- Computed style measurements
- Performance timing
- Element counts and statistics

### Visual Debugging Features
All tests include:
- Color-coded visual guides (red lines for baselines, blue for centers, etc.)
- Hover effects to highlight test areas
- Click-to-copy functionality for CSS properties
- Computed value displays
- Interactive controls (in line-height test)

## Common Issues to Look For

### Image Truncation Issues
- Blurry or jagged edges where images are clipped
- Images extending beyond container boundaries
- Inconsistent behavior between SVG and raster images
- Performance problems with many clipped images

### Text Alignment Issues  
- Chinese/CJK characters appearing lower than Latin text
- Inconsistent baseline alignment across font sizes
- Incorrect vertical-align property behavior
- Flexbox/grid alignment not working as expected

### Line Height Issues
- Incorrect spacing calculation for different units
- Inheritance not working properly
- Performance problems with many text lines
- Zero or negative line-height edge cases

## Browser Comparison Testing

To test JSAR Runtime against standard browsers:

1. **Load test files in multiple browsers**:
   - Chrome (Blink engine)
   - Firefox (Gecko engine) 
   - Safari (WebKit engine)
   - JSAR Runtime

2. **Compare visual output**:
   - Take screenshots of each test section
   - Measure pixel-perfect alignment differences
   - Note any obvious visual discrepancies

3. **Check console output**:
   - Compare computed style values
   - Verify image loading behavior
   - Check performance metrics

4. **Interactive testing**:
   - Use the line-height interactive demo
   - Test hover effects and visual guides
   - Verify click-to-copy functionality

## Expected Outcomes

These tests should help identify and fix:
- Text positioning issues (particularly the "text sinking too low" problem)
- Image clipping and overflow handling
- Line-height calculation accuracy
- Cross-platform rendering consistency

The comprehensive nature of these tests ensures that edge cases and complex scenarios are covered, providing confidence in JSAR Runtime's text and image rendering capabilities.