# Scroll Edge Shadow Indicators

This document describes the scroll edge shadow indicators feature that provides visual hints about scrollability in scrollable containers.

## Overview

Scroll edge shadow indicators render subtle shadows at the edges of scrollable containers to help users identify which edges can be scrolled. The shadows appear dynamically based on the current scroll position and available scrolling space.

## Features

- **Automatic shadow rendering**: Shadows appear automatically for scrollable containers
- **Dynamic visibility**: Shadows show/hide based on scroll position and remaining scroll space
- **Proportional sizing**: Shadow height is proportional to element dimensions (default: 20% max height)
- **Configurable appearance**: Shadow color and intensity can be customized
- **Performance optimized**: Uses SDF-based rendering for crisp, resolution-independent shadows

## Technical Implementation

### Shader-Based Rendering

The feature extends the existing SDF-based rendering system in `web_content.frag`:

```glsl
// Calculate scroll shadow alpha based on scroll position and edge proximity
vec4 calculateScrollShadows(vec2 p, vec2 dimensions, vec2 scrollOffset, vec2 contentSize, float maxHeight)
{
  // Returns shadow alpha for each edge: (top, right, bottom, left)
  // - Top/left shadows: visible when scrolled away from edges
  // - Bottom/right shadows: visible when more scrolling is available
  // - Shadow intensity decreases near element edges
}
```

### Instance Data Structure

New fields added to `InstanceData`:

```cpp
struct InstanceData {
  // ... existing fields ...
  
  glm::vec4 scrollShadowColor;    // RGBA color for scroll edge shadows
  float scrollShadowMaxHeight;    // Maximum shadow height as proportion (default 0.2 = 20%)
  glm::vec2 scrollOffset;         // Current scroll position (x, y)
  glm::vec2 contentSize;          // Content dimensions for scroll calculations
};
```

### API Methods

New methods added to `Instance` class:

```cpp
class Instance {
public:
  // Set scroll shadow appearance
  void setScrollShadowColor(glm::vec4 shadowColor);
  void setScrollShadowColor(float r, float g, float b, float a);
  void setScrollShadowMaxHeight(float maxHeight);
  
  // Update scroll state (automatically called by scroll system)
  void setScrollOffset(glm::vec2 offset);
  void setScrollOffset(float x, float y);
  void setContentSize(glm::vec2 size);
  void setContentSize(float width, float height);
};
```

## Usage

### Automatic Operation

Scroll shadows work automatically for any scrollable container (`overflow: auto` or `overflow: scroll`):

```css
.scrollable-container {
  width: 300px;
  height: 200px;
  overflow: auto; /* Enables scroll shadows automatically */
}
```

The system automatically:
1. Detects scroll containers during layout
2. Updates shadow properties when scrolling occurs
3. Renders shadows based on current scroll state

### Customization

Shadow appearance can be customized programmatically:

```cpp
// Get the instance for a scroll container
auto& instance = getInstanceForElement(scrollContainer);

// Customize shadow color (dark semi-transparent by default)
instance.setScrollShadowColor(0.2f, 0.2f, 0.2f, 0.4f);

// Adjust maximum shadow height (proportion of element size)
instance.setScrollShadowMaxHeight(0.15f); // 15% instead of default 20%
```

## Shadow Behavior

### Edge Detection

Shadows appear on different edges based on scroll state:

- **Top Shadow**: Visible when `scrollOffset.y > 0` (scrolled down)
- **Right Shadow**: Visible when `scrollOffset.x > 0` (scrolled left)  
- **Bottom Shadow**: Visible when `scrollOffset.y < maxScrollY` (can scroll down)
- **Left Shadow**: Visible when `scrollOffset.x < maxScrollX` (can scroll left)

### Gradient Effect

Shadows have a gradient effect that:
- Starts at maximum intensity at the center of the element edge
- Decreases towards the sides to create a natural shadow effect  
- Fades to zero when approaching the scroll limits

### Performance

The implementation is optimized for performance:
- Uses existing SDF rendering pipeline
- No additional draw calls
- Minimal per-pixel computation
- Automatic updates only when scroll state changes

## Browser Compatibility

This feature is built into the JSAR runtime and works consistently across all supported platforms (macOS, Android) without requiring any browser-specific code.

## Examples

### Basic Scrollable List

```html
<div style="width: 200px; height: 300px; overflow: auto;">
  <div style="height: 600px;">
    <!-- Content taller than container -->
    <p>Item 1</p>
    <p>Item 2</p>
    <!-- ... more items ... -->
  </div>
</div>
```

This will automatically show:
- Bottom shadow initially (can scroll down)
- Top shadow when scrolled down
- No shadows when scrolled to limits

### Horizontal Scrolling

```html
<div style="width: 300px; height: 100px; overflow-x: auto; white-space: nowrap;">
  <span style="display: inline-block; width: 150px;">Item 1</span>
  <span style="display: inline-block; width: 150px;">Item 2</span>
  <span style="display: inline-block; width: 150px;">Item 3</span>
  <span style="display: inline-block; width: 150px;">Item 4</span>
</div>
```

This will show left/right shadows based on horizontal scroll position.

## Future Enhancements

Potential improvements for future versions:

- CSS properties for controlling shadow appearance
- Different shadow styles (linear, radial gradients)
- Animation transitions when shadows appear/disappear
- Per-axis shadow control (horizontal vs vertical)