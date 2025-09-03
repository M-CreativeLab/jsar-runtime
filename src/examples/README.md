# JSAR Runtime Examples

This directory contains examples demonstrating the JSAR Runtime capabilities with the new TransmuteBrowser architecture.

## Bar Component Refactoring

The bar component has been refactored to improve separation of concerns and code modularity. See the detailed refactoring documentation below.

### Architecture Overview

The monolithic `BarComponent` has been split into three specialized components:

1. **ContentBarCanvas** - Handles UI rendering with Skia Canvas and Apple-style design
2. **ContentBar3d** - Handles 3D scene integration with OpenGL rendering
3. **EventProxy** - Manages event forwarding between components

The original `BarComponent` now acts as a facade that maintains API compatibility while coordinating the specialized components.

### Key Benefits

- **Separation of Concerns**: 3D scene logic completely separated from UI rendering logic
- **Event Decoupling**: Canvas subcomponents can handle events independently
- **API Compatibility**: Existing code continues to work without changes
- **Enhanced Extensibility**: Easy to extend both 3D and UI functionality independently

### Usage

```cpp
// Existing API continues to work
BarComponent barComponent;
barComponent.addContent(content);
barComponent.setContentHovered(content, true);

// Advanced usage - access individual components
auto canvas = barComponent.getCanvasComponent();
auto bar3d = barComponent.get3dComponent();
auto eventProxy = barComponent.getEventProxy();
```

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
- **Control Options**:
  - `--env-map` - Enable environment map (default)
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

# Stereo rendering with environment map
./transmute_browser --stereo singlepass

# Disable environment map for solid black background
./transmute_browser --no-env-map

# Custom window size with environment map
./transmute_browser -w 1920 -h 1080 --env-map
```
