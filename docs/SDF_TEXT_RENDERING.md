# SDF Text Rendering

This implementation provides Signed Distance Field (SDF) based text rendering for the JSAR runtime, enabling crisp, scalable text in 3D environments through integration with the existing WebContent texture system.

## Features

- **Integrated SDF rendering** through existing `RenderTextSystem::render()` method
- **WebContent material support** for SDF anti-aliasing via instance textures
- **Always enabled** for optimal text quality and anti-aliasing
- **Seamless integration** - existing text rendering enhanced with SDF support
- **Generic SDF support** - can extend to SVG images and other instance textures
- **CPU-based SDF generation** using TinySDF algorithm (when needed)

## Architecture

### Integration Approach

Instead of a separate SDF rendering system, this implementation enhances the existing text rendering pipeline:

1. **`RenderTextSystem::render()`** - Enhanced to generate SDF-based text for optimal anti-aliasing
2. **`WebContentInstancedMaterial`** - Extended to support SDF anti-aliasing for instance textures
3. **Existing `Text2d` components** - No separate component types needed
4. **WebContent texture pipeline** - SDF text rendered to same texture system as regular content

### Core Components

1. **Enhanced RenderTextSystem** (`src/client/builtin_scene/web_content_renderer.cpp`)
   - `renderTextWithSDF()` method for SDF text generation
   - Integration with existing Skia text layout
   - Always enabled for optimal text quality

2. **WebContentInstancedMaterial** (`src/client/builtin_scene/materials/web_content_instanced.cpp`)
   - `setSdfEnabled()` method for SDF mode control
   - Shader support for SDF anti-aliasing
   - Works with existing instance texture system

3. **TinySDF Utilities** (`src/client/builtin_scene/text/sdf/`)
   - `TinySDF.cpp` - CPU-based SDF generation from font glyphs
   - `Atlas.cpp` - Bin-packing for texture atlas creation  
   - `Cache.cpp` - LRU cache for SDF textures

### Shader Integration

The existing `web_content.frag` shader already supports SDF rendering:
- `uSdfEnabled` uniform controls SDF anti-aliasing mode
- SDF distance-based alpha calculation for smooth edges
- Works with instance textures for batched rendering
  - Configurable edge softness and thickness
  - Color and opacity control

## Usage

### Usage

Text anti-aliasing is now always enabled through the integrated SDF approach. No configuration is needed as SDF rendering is automatically applied to all text content for optimal quality.

All text rendered through `RenderTextSystem::render()` will use SDF anti-aliasing via the `WebContentInstancedMaterial`.

### Integration Details

The SDF text system integrates seamlessly with existing code:

1. **Layout System**: Always creates `Text2d` components (no separate component types)
2. **Render System**: `RenderTextSystem::render()` automatically uses SDF rendering
3. **Material System**: `WebContentInstancedMaterial::setSdfEnabled()` enables SDF anti-aliasing
4. **Texture System**: SDF text rendered to same instance texture atlas as other content

### Configuration

SDF generation parameters are configured automatically based on text style:

```cpp
// Font size and family extracted from existing WebContent style
auto textStyle = content.paragraphStyle().getTextStyle();
int fontSize = static_cast<int>(textStyle.getFontSize());

// SDF parameters derived from font size
text::sdf::SDFParams sdfParams(fontSize, 8, 8, 0.25f);
```

### Material Properties

SDF anti-aliasing is controlled via the WebContent material:

```cpp
auto webContentMaterial = material3d->material<materials::WebContentInstancedMaterial>();
webContentMaterial->setSdfEnabled(true);  // Enable SDF anti-aliasing
```

## Performance Considerations

- **Always Enabled**: SDF text anti-aliasing is now always enabled for optimal text quality
- **Instance Texture Integration**: SDF text uses same atlas/batching as other content  
- **Shader Efficiency**: Single shader handles SDF content with optimal performance
- **Generic SDF Support**: Infrastructure can extend to SVG images and other content types

## Testing

Test SDF text integration:

```bash
# Build and run tests (when build system is configured)
# Tests cover SDF generation, atlas packing, and caching
```

View demonstration:

```bash
# Open fixtures/html/text-antialiasing.html in JSAR runtime
# SDF text anti-aliasing is always enabled for optimal quality
```

## Limitations

- Currently CPU-based SDF generation (could be GPU-accelerated)
- Single-channel SDF (could upgrade to MSDF for better quality)
- Basic UTF-8 support (complex scripts may need advanced shaping)
- Font loading relies on existing Skia font system

## Future Enhancements

1. **MSDF Support**: Multi-channel SDF for higher quality at large scales
2. **GPU Generation**: Move SDF generation to compute shaders
3. **Advanced Shaping**: Integration with HarfBuzz for complex scripts
4. **Effect Support**: Outline, drop shadow, glow effects
5. **Global Font Atlas**: Shared atlas across multiple text nodes

## Implementation Notes

- Integrates with existing `LayoutText` system via feature flag
- Follows JSAR ECS patterns with `SDFText2d` component
- Uses existing material/shader architecture
- Maintains compatibility with current text layout engine
- Minimal changes to existing codebase (additive implementation)