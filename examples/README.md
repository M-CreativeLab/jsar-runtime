# JSAR Runtime Examples

This directory contains example code demonstrating how to use various features of the JSAR Runtime.

## Gradient Usage Example

### File: `gradient_usage_example.cpp`

This example demonstrates the gradient data extraction API that allows developers to:

- Extract gradient information from computed CSS image values
- Render linear and radial gradients using Skia
- Handle different gradient directions and properties
- Process color stops and interpolation hints

**Key Functions:**

- `processBackgroundImage()` - Main function to detect and route different image types
- `renderLinearGradient()` - Process linear gradient properties
- `renderRadialGradient()` - Process radial gradient properties  
- `drawGradientToCanvas()` - Complete Skia integration example
- `createTestLinearGradient()` - Create gradients programmatically for testing
- `inspectGradient()` - Debug/inspect gradient properties

**Usage Patterns:**

The example shows how to:
1. Check if an image is a gradient using `image.isGradient()`
2. Extract specific gradient types using `getLinearGradient()` and `getRadialGradient()`
3. Access gradient properties like direction, shape, size, and color stops
4. Convert gradient data to Skia shaders for rendering
5. Handle edge cases and fallbacks

**Integration:**

This example code can be integrated into web content renderers, layout engines, or any graphics system that needs to render CSS gradients. The pattern shown is used in the actual `web_content_renderer.cpp` implementation.

**Building:**

Note: These examples require the full JSAR Runtime build environment with Skia and other dependencies. They are provided for reference and documentation purposes.

## Future Examples

Additional examples may be added for:
- Image loading and processing
- CSS animation and transitions
- Layout and positioning
- Web API bindings
- 3D rendering integration