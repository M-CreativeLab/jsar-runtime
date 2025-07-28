# SDF-Based Web Content Rendering Test Guide

This document describes how to test the SDF-based rendering improvements for Web Content planes.

## What's New

The Web Content rendering now uses Signed Distance Fields (SDF) to achieve superior anti-aliasing on plane edges and borders. This is particularly noticeable on:

1. Rounded corners/borders
2. Plane edges when scaled or rotated
3. High DPI displays
4. Zoomed content

## Key Features

- **SDF-based anti-aliasing**: Smooth edges computed mathematically in the fragment shader
- **Border radius support**: Accurate SDF calculations for rounded rectangles
- **Configurable anti-alias width**: Adjustable smoothing zone
- **Backward compatibility**: Can be enabled/disabled per content

## Configuration

### Enable/Disable SDF Rendering
```cpp
// In your web content material setup
webContentMaterial->setSdfEnabled(true);  // Enable SDF rendering
webContentMaterial->setSdfEnabled(false); // Use traditional texture-only rendering
```

### Adjust Anti-Alias Width
```cpp
// Set anti-alias width (in logical units)
webContentMaterial->setSdfAntiAliasWidth(2.0f);  // Default value
webContentMaterial->setSdfAntiAliasWidth(1.0f);  // Sharper edges
webContentMaterial->setSdfAntiAliasWidth(4.0f);  // More blurred edges
```

## Testing Scenarios

1. **Simple Rectangle**: Create web content without border radius
2. **Rounded Rectangle**: Test with various border radius values
3. **Scaled Content**: Scale web content planes and observe edge quality
4. **Rotated Content**: Rotate planes to test diagonal edge quality
5. **High DPI**: Test on high-resolution displays

## Visual Quality Comparison

### Before (Traditional Texture Sampling)
- Aliasing artifacts on edges
- Jagged corners on scaled content
- Limited anti-aliasing quality

### After (SDF-Based Rendering)
- Smooth, mathematically perfect edges
- Crisp corners at any scale
- Superior anti-aliasing quality

## Performance Considerations

- SDF calculations add minimal fragment shader overhead
- Most beneficial for content with visible edges (rounded corners, scaled planes)
- Can be disabled for performance-critical scenarios where edge quality is not important