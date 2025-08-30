# JSAR Runtime Examples

This directory contains examples demonstrating the JSAR Runtime capabilities with the new TransmuteBrowser architecture.

## Desktop OpenGL Example

The `desktop_opengl` example demonstrates a complete spatial web browser with screen-space GUI.

### Architecture Overview

The example has been refactored from a monolithic `App` class to a modular `TransmuteBrowser` architecture:

#### Core Components

1. **TransmuteBrowser** (`transmute_browser.hpp/.cpp`)
   - Main browser class managing content and GUI
   - Replaces the old `App` class
   - Handles window management, embedder lifecycle, and rendering coordination

2. **ScreenRenderer** (`screen_renderer.hpp/.cpp`)  
   - Manages all screen-space GUI components
   - Provides component lifecycle and input handling
   - Renders components in correct layered order

3. **Content** (`content.hpp/.cpp`)
   - Wraps `TrContentRuntime` for spatial content management
   - Provides spatial positioning and dragging capabilities
   - Each content instance has an associated bar component

4. **BarComponent** (`bar_component.hpp/.cpp`)
   - Visual bar rendered beneath each content for dragging
   - Handles mouse interaction for spatial movement
   - Provides visual feedback during dragging

5. **InputBox** (`input_box.hpp/.cpp`)
   - URL input component at bottom of window
   - Supports text editing, cursor movement, and submission
   - Integrates with screen renderer for input handling

6. **StatPanel** (`stat_panel.hpp`)
   - Performance statistics display (refactored to use ScreenComponent)
   - Shows FPS and uptime information

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
# Build (macOS only)
make darwin

# Run with default settings
./build/examples/desktop_opengl

# Run with specific configuration
./build/examples/desktop_opengl --stereo --fps 60 -n 2

# Stereo rendering with environment map
./build/examples/desktop_opengl --stereo singlepass

# Disable environment map for solid black background
./build/examples/desktop_opengl --no-env-map

# View all options
./build/examples/desktop_opengl --help
```

### New GUI Controls
- **Enter URL**: Click input box at bottom and type URLs, press Enter to open
- **Drag Content**: Click and drag content bars to move content spatially
- **Focus Input**: Click input box to focus for typing

### API Reference

#### TransmuteBrowser Content Management

```cpp
// Open new content
auto content = browser.openContent("http://example.com/app.xsml");

// Close content by ID
browser.closeContent(contentId);

// Find content by ID
auto content = browser.getContentById(contentId);

// Find content near position
auto content = browser.getContentByPosition(glm::vec3(0, 0, 0.5), 0.1f);

// Get all content instances
auto allContent = browser.getAllContents();
```
