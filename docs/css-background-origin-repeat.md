# CSS Background-Origin and Background-Repeat Implementation

This document summarizes the implementation of CSS `background-origin` and `background-repeat` properties in the JSAR runtime.

## Implementation Overview

The implementation follows the established JSAR CSS architecture pattern with minimal, surgical changes:

### File Structure
```
src/client/cssom/values/
├── generics/background.hpp          # Generic template classes
├── specified/background.hpp         # Parsing logic
├── computed/background.hpp          # Computed values
├── computed_style.hpp              # Style container
└── computed_style.cpp              # Property parsing

src/client/builtin_scene/
└── web_content_renderer.cpp        # Rendering implementation

tests/client/
└── css_background_properties_tests.cpp  # Unit tests

fixtures/html/
├── background-origin-repeat-test.html    # Demonstration fixture
└── simple.html                          # Updated showcase
```

## Background-Origin Property

Defines the positioning area for background images:

- `padding-box` (default): Position relative to padding box
- `border-box`: Position relative to border box  
- `content-box`: Position relative to content box

### Implementation Details
- Calculates positioning area by subtracting appropriate border/padding widths
- Uses `style.borderWidth().{side}().value()` and `style.padding().{side}().value()` accessors
- Properly integrates with existing clipping and rendering pipeline

## Background-Repeat Property

Controls how background images are tiled:

- `repeat` (default): Tile both horizontally and vertically
- `repeat-x`: Tile only horizontally
- `repeat-y`: Tile only vertically
- `no-repeat`: Display once without tiling
- `space`, `round`: Parsed but default to no-repeat behavior

### Implementation Details
- Efficient tiling algorithm using nested loops for appropriate directions
- Proper clipping to positioning area boundaries
- Calculates source rectangles for partial tiles at boundaries
- Maintains image aspect ratio and quality

## Architecture Integration

### Parsing Pipeline
1. CSS property parsed by specified class
2. Converted to computed value with context
3. Stored in ComputedStyle with appropriate getter
4. Used during rendering to calculate positioning and tiling

### Rendering Pipeline
1. `getBackgroundPositioningArea()` calculates positioning based on origin
2. `drawBackgroundImage()` handles tiling based on repeat value
3. Proper integration with existing background features (blend-mode, clip)
4. Maintains performance with efficient algorithms

## Browser Compatibility

The implementation follows CSS specifications and matches mainstream browser behavior:

- Default values match CSS defaults
- Box model calculations follow CSS box model specification
- Tiling patterns match expected browser behavior
- Graceful fallbacks for unsupported `space`/`round` values

## Testing

Comprehensive test coverage includes:

- Unit tests for all property values
- Parsing validation tests
- Computed value conversion tests
- Interactive demonstration fixtures
- Integration with existing background features

## Usage Examples

```css
/* Background positioned in content area, tiled horizontally */
.example {
  background-image: url(image.jpg);
  background-origin: content-box;
  background-repeat: repeat-x;
}

/* Background positioned in border area, no tiling */
.example2 {
  background-image: url(pattern.png);
  background-origin: border-box;
  background-repeat: no-repeat;
}
```

## Performance Considerations

- Efficient tiling algorithm with minimal overdraw
- Proper clipping reduces unnecessary rendering
- Compatible with existing Skia rendering optimizations
- No impact on elements without background images

This implementation successfully adds modern CSS background functionality to JSAR while maintaining compatibility and performance.