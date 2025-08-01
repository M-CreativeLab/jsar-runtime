# Environment Map Rendering in JSAR Simulator

This document describes the default environment map rendering feature added to the JSAR desktop simulator example.

## Overview

The simulator now supports automatic environment map rendering that provides realistic background colors and lighting based on the viewer's orientation. This enhances the visual experience by replacing the previous solid black background with dynamic colors that simulate sky, horizon, and ground.

## Features

- **Automatic Environment Colors**: Dynamic background colors that change based on head orientation
  - **Sky**: Light blue color when looking up
  - **Horizon**: Warm orange/yellow color when looking horizontally  
  - **Ground**: Brown color when looking down

- **Command-Line Control**: Easy enable/disable via command-line options

- **XR Compatibility**: Works with both mono and stereo rendering modes, as well as multipass and singlepass rendering

- **Minimal Performance Impact**: Uses simple color calculation instead of complex geometry rendering

## Command-Line Options

### Enable Environment Map (Default)
```bash
jsar_desktop_opengl --env-map
```
or simply (enabled by default):
```bash
jsar_desktop_opengl
```

### Disable Environment Map
```bash
jsar_desktop_opengl --no-env-map
```

### Combined with Other Options
```bash
# Stereo rendering with environment map
jsar_desktop_opengl --stereo singlepass --env-map

# Mono rendering without environment map
jsar_desktop_opengl --mono --no-env-map
```

## Implementation Details

### Architecture

The environment map feature is implemented through:

1. **EnvironmentRenderer Class** (`src/examples/environment_renderer.hpp/cpp`)
   - Manages environment map state and rendering logic
   - Analyzes view matrix to determine orientation
   - Calculates appropriate background colors

2. **Integration in Desktop Simulator** (`src/examples/desktop_opengl.cpp`)
   - Command-line argument parsing for `--env-map`/`--no-env-map`
   - Initialization and cleanup of environment renderer
   - Dynamic background color application during rendering

### Color Logic

The environment renderer uses the view matrix to determine the viewing direction:

```cpp
// Extract vertical component from view matrix
float upComponent = viewMatrix[1][2];

if (upComponent > 0.0f) {
    // Looking up - sky color (light blue)
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
} else if (upComponent < -0.3f) {
    // Looking down - ground color (brown)
    glClearColor(0.3f, 0.2f, 0.1f, 1.0f);
} else {
    // Horizon - warm color (orange/yellow)
    glClearColor(1.0f, 0.9f, 0.7f, 1.0f);
}
```

### Rendering Pipeline Integration

The environment rendering is integrated into the main rendering loop:

1. **Framebuffer Binding**: After binding the render target framebuffer
2. **Environment Analysis**: If environment map is enabled, analyze view direction
3. **Color Application**: Set appropriate clear color based on orientation
4. **Buffer Clear**: Clear with the calculated environment color
5. **Content Rendering**: Proceed with normal JSAR content rendering

## Customization

### Extending the Environment Renderer

To add more sophisticated environment mapping:

1. **Texture-Based Environments**: Load HDRI or cubemap textures
2. **Shader-Based Rendering**: Implement proper skybox rendering with vertex/fragment shaders
3. **Time-of-Day Variations**: Add dynamic lighting based on time
4. **Weather Effects**: Implement cloudy, rainy, or other weather conditions

### Example Extension

```cpp
// In environment_renderer.cpp
void EnvironmentRenderer::loadHDRI(const std::string& hdriPath) {
    // Load HDRI texture
    // Create skybox geometry
    // Set up shaders for proper environment mapping
}
```

## Compatibility

- **Rendering Modes**: Compatible with mono/stereo and multipass/singlepass
- **Platforms**: Works on all platforms supported by the desktop simulator
- **Performance**: Minimal impact - only adds a single color calculation per frame
- **Existing Content**: Does not interfere with existing JSAR content rendering

## Troubleshooting

### Environment Map Not Visible
- Check that `--no-env-map` is not specified
- Verify that XR rendering is enabled (non-XR mode not supported)
- Ensure the view is not completely occluded by content

### Colors Not Changing
- Verify head tracking is working properly
- Check that the view matrices are being updated correctly
- Ensure the environment renderer is properly initialized

## Future Enhancements

1. **Cubemap Support**: Load and render actual cubemap textures
2. **HDRI Integration**: Support for high dynamic range imaging
3. **Procedural Skies**: Generate realistic sky patterns with clouds and sun
4. **Environment Probes**: IBL (Image-Based Lighting) for realistic reflections
5. **Asset Pipeline**: Tools for converting and optimizing environment maps

## API Reference

### EnvironmentRenderer Class

```cpp
class EnvironmentRenderer {
public:
    EnvironmentRenderer();
    ~EnvironmentRenderer();
    
    bool initialize();
    void render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
    void shutdown();
    
    void setEnabled(bool enabled);
    bool isEnabled() const;
};
```

### Command-Line Arguments

| Option | Description | Default |
|--------|-------------|---------|
| `--env-map` | Enable environment map rendering | Enabled |
| `--no-env-map` | Disable environment map rendering | - |

## Related Files

- `src/examples/environment_renderer.hpp` - Environment renderer header
- `src/examples/environment_renderer.cpp` - Environment renderer implementation  
- `src/examples/desktop_opengl.cpp` - Simulator integration
- `fixtures/html/three/cubemap-mipmap.js` - Example of cubemap usage in Three.js content