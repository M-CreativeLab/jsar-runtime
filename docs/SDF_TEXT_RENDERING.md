# SDF Text Rendering

This implementation provides Signed Distance Field (SDF) based text rendering for the JSAR runtime, enabling crisp, scalable text in 3D environments.

## Features

- **CPU-based SDF generation** using TinySDF algorithm
- **Texture atlas packing** for efficient GPU memory usage
- **LRU cache** to avoid regenerating SDF textures
- **OpenGL shader** with configurable thickness and softness
- **ECS integration** following existing JSAR patterns
- **Unicode support** via existing Skia font system

## Architecture

### Core Components

1. **TinySDF** (`src/client/builtin_scene/text/sdf/TinySDF.cpp`)
   - CPU-based SDF generation from font glyphs
   - Uses Skia for font rasterization and metrics
   - Configurable distance field parameters

2. **Atlas** (`src/client/builtin_scene/text/sdf/Atlas.cpp`)
   - Bin-packing algorithm for texture atlas creation
   - UV coordinate mapping for glyph positioning
   - Efficient memory layout for GPU upload

3. **Cache** (`src/client/builtin_scene/text/sdf/Cache.cpp`)
   - LRU cache for SDF textures
   - Keyed by font properties and text content
   - Configurable memory limits

4. **SDFTextRenderer** (`src/client/builtin_scene/text/SDFTextRenderer.cpp`)
   - Main integration point with ECS system
   - Mesh generation for text quads
   - GPU texture management

5. **SDFTextMaterial** (`src/client/builtin_scene/materials/sdf_text.cpp`)
   - OpenGL material for SDF text rendering
   - Shader uniforms for appearance control
   - Integration with existing material system

### Shaders

- **Vertex Shader** (`src/client/builtin_scene/shaders/sdf_text.vert`)
  - Standard vertex transformation
  - UV coordinate passing

- **Fragment Shader** (`src/client/builtin_scene/shaders/sdf_text.frag`)
  - SDF sampling and distance-based alpha calculation
  - Configurable edge softness and thickness
  - Color and opacity control

## Usage

### Enabling SDF Text

Set the environment variable to enable SDF text rendering:

```bash
export JSAR_ENABLE_SDF_TEXT=1
```

When enabled, all text nodes will use SDF rendering instead of regular text rendering.

### Configuration

SDF generation parameters can be configured via `SDFParams`:

```cpp
sdf::SDFParams params;
params.fontSize = 24;    // Font size in pixels
params.buffer = 8;       // Buffer around glyph
params.radius = 8;       // Distance field radius
params.cutoff = 0.25f;   // Alpha cutoff for edge detection
```

### Material Properties

SDF text appearance can be controlled via material properties:

```cpp
auto material = materials::SDFTextMaterial::create(textureId);
material->setSDFSpread(8.0f);        // Distance field spread
material->setSDFThickness(0.5f);     // Text thickness (0.5 = normal)
material->setSDFSoftness(1.0f);      // Edge softness
material->setTextColor(1.0f, 1.0f, 1.0f, 1.0f);  // RGBA color
material->setOpacity(1.0f);          // Additional opacity
```

## Performance Considerations

- **Cache Efficiency**: SDF textures are cached by font and content hash
- **Memory Management**: LRU eviction prevents unlimited memory growth  
- **Batch Rendering**: Multiple glyphs can be rendered in single draw call
- **Asynchronous Generation**: SDF generation could be moved to background thread

## Testing

Run SDF text tests:

```bash
# Build and run tests (when build system is configured)
# Tests cover SDF generation, atlas packing, and caching
```

View demonstration:

```bash
# Open fixtures/html/sdf-text-demo.html in JSAR runtime
# Set JSAR_ENABLE_SDF_TEXT=1 to see SDF rendering
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