# JSAR Runtime Examples

This directory contains examples demonstrating the JSAR Runtime capabilities with the new TransmuteBrowser architecture.

## Transmute Browser

The `desktop_opengl` example demonstrates a complete spatial web browser example.

### Key Features

#### Screen-Space GUI

- **Input Box**: Enter URLs to open new content instances
- **Statistics Panel**: Shows real-time performance data  
- **Component System**: Extensible architecture for adding new GUI elements

#### Spatial Content Management

- **Multiple Content**: Support for multiple spatial content instances
- **Drag and Drop**: Click and drag content bars to reposition content spatially
- **Grid Layout**: Automatic positioning of new content in 3x3 grid
- **Content API**: Programmatic access to content by ID or position

#### Enhanced Input Handling

- **Text Input**: Full text editing support in input components
- **Mouse Interaction**: Content dragging and GUI interaction
- **Keyboard Shortcuts**: Frame rate controls and application commands

### Environment Map Rendering

- **Default Environment**: Automatically renders realistic background colors based on viewing direction
- **Dynamic Colors**: 
  - Sky blue when looking up
  - Warm horizon colors when looking horizontally
  - Earth brown when looking down
- **360° Panorama Support**: Load PNG/JPG panorama images with equirectangular projection
- **Cube Map Support**: Load traditional cube map directories with 6 face images
- **Control Options**:
  - `--env-map /path/to/panorama.jpg` - Load a 360° panorama image
  - `--env-map /path/to/cubemap/` - Load cube map from directory
  - `--env-map` - Use default procedural environment
  - `--no-env-map` - Disable environment map for solid black background

### XR Rendering Support

- **Mono Mode**: `--mono` - Single eye rendering
- **Stereo Mode**: `--stereo [mode]` - Dual eye rendering
  - `singlepass` - Single rendering pass (default)
  - `multipass` - Multiple rendering passes

### Usage

```bash
## Default with environment map
./transmute_browser

# Load a 360° panorama image
./transmute_browser --env-map /path/to/panorama.jpg

# Load cube map from directory
./transmute_browser --env-map /path/to/cubemap_directory/

# Stereo rendering with panorama
./transmute_browser --stereo singlepass --env-map /path/to/panorama.jpg

# Disable environment map for solid black background
./transmute_browser --no-env-map

# Custom window size with panorama
./transmute_browser -w 1920 -h 1080 --env-map /path/to/panorama.jpg
```
