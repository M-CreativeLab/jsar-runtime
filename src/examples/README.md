# Examples

This directory contains examples of how to embed the `jsar-runtime` library with platforms and game engines.

## Desktop OpenGL Simulator (`desktop_opengl.cpp`)

A desktop simulator for JSAR runtime content with the following features:

### Environment Map Rendering
- **Default Environment**: Automatically renders realistic background colors based on viewing direction
- **Dynamic Colors**: 
  - Sky blue when looking up
  - Warm horizon colors when looking horizontally
  - Earth brown when looking down
- **Control Options**:
  - `--env-map` - Enable environment map (default)
  - `--no-env-map` - Disable environment map for solid black background

### XR Rendering Support
- **Mono Mode**: `--mono` - Single eye rendering
- **Stereo Mode**: `--stereo [mode]` - Dual eye rendering
  - `singlepass` - Single rendering pass (default)
  - `multipass` - Multiple rendering passes

### Usage Examples

```bash
# Default with environment map
./jsar_desktop_opengl

# Stereo rendering with environment map
./jsar_desktop_opengl --stereo singlepass

# Disable environment map for solid black background
./jsar_desktop_opengl --no-env-map

# Custom window size with environment map
./jsar_desktop_opengl -w 1920 -h 1080 --env-map
```

### Testing Environment Maps

Use the provided demo file to test environment mapping:

```bash
./jsar_desktop_opengl fixtures/html/environment-map-demo.html
```

Look around in different directions to see the background colors change dynamically.

For more details, see [Environment Map Documentation](../docs/environment-map-rendering.md).
