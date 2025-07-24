# Background-Clip: Text Implementation

This document describes the implementation of the `background-clip: text` CSS property in the JSAR runtime.

## Overview

The `background-clip: text` property allows background colors, gradients, and images to be clipped to the foreground text, creating visually rich text effects where the background is only visible within the text glyphs.

## Implementation Details

### Files Modified

1. **`src/client/builtin_scene/web_content_renderer.cpp`**
   - Added `createTextPath()` helper function to extract text bounds
   - Modified `drawBackground()` function to handle text clipping
   - Implemented clipping for background colors, gradients, and images

2. **`tests/client/css_background_properties_tests.cpp`**
   - Added unit tests for `BackgroundClip` parsing including the "text" value

3. **`fixtures/html/background-clip-text-test.html`**
   - Comprehensive test page with various background-clip scenarios

### Technical Approach

#### Text Path Creation
```cpp
optional<SkPath> createTextPath(const std::string &textContent,
                               const WebContent &content)
```

The `createTextPath` function:
1. Creates a paragraph to get text layout using Skia's paragraph system
2. Extracts the font typeface and size from the text style
3. Converts text to glyph IDs using the font's text-to-glyph conversion
4. Creates actual glyph paths from font shapes using `font.getPath()`
5. Positions each glyph correctly and adds it to the final text path
6. Falls back to rectangular approximation if font/typeface is unavailable
7. Handles edge cases like empty text content

#### Background Rendering
The `drawBackground` function was enhanced to:
1. Check if `background-clip: text` is specified
2. Create a text clipping path when needed
3. Apply the clipping before drawing backgrounds
4. Handle all background types (colors, gradients, images)

### Supported Features

- ✅ Solid color backgrounds with text clipping
- ✅ Linear gradients with text clipping
- ✅ Radial gradients with text clipping
- ✅ Background images with text clipping
- ✅ Proper handling of empty text (no background drawn)
- ✅ Multi-line text support
- ✅ Integration with existing background-origin and background-repeat properties

### CSS Usage Examples

```css
/* Solid color text effect */
.text-clip-solid {
    background-color: #ff6b6b;
    background-clip: text;
    color: transparent;
}

/* Gradient text effect */
.text-clip-gradient {
    background: linear-gradient(45deg, #ff6b6b, #4ecdc4);
    background-clip: text;
    color: transparent;
}

/* Multiple lines with gradient */
.text-clip-multiline {
    background: linear-gradient(to right, red, blue, green);
    background-clip: text;
    color: transparent;
}
```

### Testing

#### Unit Tests
Run the CSS background properties tests to verify parsing:
```bash
# Run tests (when build system is available)
make test
```

#### Integration Tests
Open `fixtures/html/background-clip-text-test.html` in a JSAR-enabled browser to see:
- Various background types with text clipping
- Edge cases like empty text elements
- Comparison with other background-clip values

### Current Limitations

1. **Fallback Behavior**: When font typeface is unavailable, the implementation falls back to rectangular bounds approximation. This ensures robustness while maintaining good visual results in most cases.

2. **Texture Requirements**: Elements with `background-clip: text` require texture rendering, which is the correct behavior but may impact performance for many such elements.

3. **Single-line Optimization**: The current glyph positioning is optimized for single-line text. Multi-line text may need additional layout calculations for precise glyph positioning.

### Future Enhancements

1. **Multi-line Text Layout**: Improve glyph positioning for multi-line text by using paragraph line metrics and proper baseline calculations
2. **Advanced Typography Features**: Support for kerning, ligatures, and other advanced typography features
3. **Performance Optimizations**: Caching of glyph paths for repeated renders, especially for static text content
4. **Text Shaping**: Integration with text shaping engines for complex scripts and languages

### Browser Compatibility

The implementation follows the CSS Background and Borders Module Level 3 specification for `background-clip: text`, providing compatibility with:
- WebKit-based browsers (using `-webkit-background-clip`)
- Firefox (native `background-clip` support)
- Modern Chromium-based browsers

### Architecture Integration

The implementation integrates cleanly with JSAR's existing architecture:
- Uses the established ECS (Entity-Component-System) pattern
- Leverages existing Skia paragraph layout system
- Maintains compatibility with other CSS background properties
- Follows existing code patterns and naming conventions