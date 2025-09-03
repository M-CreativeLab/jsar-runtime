# JSAR Runtime Examples

This directory contains examples demonstrating the JSAR Runtime capabilities with the new TransmuteBrowser architecture.

## Canvas System Architecture

The bar component has been completely redesigned based on reviewer feedback to use a modern Canvas system approach.

### Architecture Overview

The new architecture consists of two main components:

1. **CanvasSystem** - Modern Skia-based GUI drawing with direct ray event handling
2. **ContentBar3d** - Mesh-based 3D component that inherits from `builtin_scene::Mesh`

The original `BarComponent` now acts as a facade that maintains API compatibility.

### Key Features

#### Canvas System
- **Skia Integration**: Uses Skia Canvas for modern GUI drawing capabilities
- **Direct Ray Events**: Processes ray move, down, and up events directly
- **Flexible Rendering**: Can be rendered via ScreenRenderer (screen-space) or MeshRenderer (3D mesh)
- **Apple-style Design**: Built-in Apple-style visual feedback and design
- **Event Handlers**: Extensible ray event handling system

#### ContentBar3d as Mesh
- **Mesh Inheritance**: Inherits from `builtin_scene::Mesh` for proper 3D integration
- **MeshRenderer Compatible**: Can be rendered using the standard MeshRenderer
- **Ray Intersection**: Advanced ray-plane intersection for 3D interaction
- **Geometry Management**: Proper vertex/index buffer management

### Usage

```cpp
// Existing API continues to work
BarComponent barComponent;
barComponent.addContent(content);
barComponent.setContentHovered(content, true);

// Access canvas system for GUI customization
auto canvas = barComponent.getCanvasSystem();
canvas->registerRayEventHandler([](const RayEvent& event) {
    // Custom ray event handling
    return true;
});

// Access 3D component as a Mesh
auto bar3d = barComponent.get3dComponent();
// bar3d can now be used with MeshRenderer
```

### Ray Event System

The canvas system provides a unified ray event interface:

```cpp
enum class RayEventType { Move, Down, Up };

struct RayEvent {
    RayEventType type;
    glm::vec3 rayOrigin;
    glm::vec3 rayDirection; 
    glm::vec2 localPosition; // Normalized 0-1 coordinates
};
```

### Benefits

- **No Event Proxy**: Eliminated complex event forwarding mechanisms
- **Direct Ray Handling**: Canvas processes ray events without intermediaries  
- **Mesh Integration**: ContentBar3d works seamlessly with existing Mesh/MeshRenderer systems
- **Simplified Architecture**: Cleaner separation between GUI and 3D concerns
- **Modern GUI**: Skia-based rendering for advanced visual effects

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
