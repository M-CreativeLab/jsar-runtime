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
./transmute_browser

# Stereo rendering with environment map
./transmute_browser --stereo singlepass

# Disable environment map for solid black background
./transmute_browser --no-env-map

# Custom window size with environment map
./transmute_browser -w 1920 -h 1080 --env-map
```
