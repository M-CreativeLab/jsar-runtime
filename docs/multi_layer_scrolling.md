# Multi-Layer Scrolling for JSAR Runtime

This implementation adds support for multi-layer scrolling to the JSAR runtime, allowing HTML 2D elements to be rendered in separate layers based on their scrollable containers.

## Overview

The multi-layer scrolling system identifies scrollable containers (elements with `overflow: scroll` or `overflow: auto`) and creates separate rendering layers for each. This enables:

- **Independent scrolling**: Each scrollable container can scroll independently without affecting others
- **Performance optimization**: Only dirty layers re-render on scroll events
- **Better compositing**: Proper depth ordering and layer management
- **Reduced overdraw**: Each layer only renders its own content

## Architecture

### Core Components

1. **ScrollLayer** (`src/client/scroll/scroll_layer.hpp/cpp`)
   - Represents a single scrollable layer
   - Contains position, size, depth, and WebContent
   - Manages its own ScrollableArea for scroll state

2. **LayerManager** (`src/client/scroll/layer_manager.hpp/cpp`)
   - Manages all scroll layers
   - Associates layers with layout objects
   - Handles layer creation, removal, and depth ordering

3. **MultiLayerRenderSystem** (`src/client/builtin_scene/multi_layer_renderer.hpp/cpp`)
   - Integrates with existing ECS rendering pipeline
   - Identifies scrollable containers during layout
   - Renders layers in correct depth order

4. **Integration API** (`src/client/scroll/multi_layer_scroll_api.hpp/cpp`)
   - Provides simple integration points for existing systems
   - Global functions for layer management and scroll routing

### Integration Points

The system integrates with existing JSAR runtime components:

- **Layout System**: Identifies scrollable containers and creates layers
- **Rendering System**: Renders each layer separately with scroll transforms
- **Input System**: Routes scroll events to appropriate layers
- **ECS System**: Uses existing Entity-Component-System architecture

## Usage

### Basic Integration

```cpp
#include <client/scroll/multi_layer_scroll_api.hpp>

// Initialize during app startup
jsar_multi_layer_integration::initializeMultiLayerScrolling();

// During layout processing
for (auto layoutObject : layoutObjects) {
    uint32_t layerId = jsar_multi_layer_scroll::createScrollLayerIfNeeded(layoutObject);
}

// After layout is complete
jsar_multi_layer_integration::onLayoutComplete();

// During rendering
auto layersToRender = jsar_multi_layer_integration::getLayersToRender();
for (uint32_t layerId : layersToRender) {
    // Render each layer with its scroll offset applied
}

// Handle scroll events
bool handled = jsar_multi_layer_integration::routeScrollEvent(mouseX, mouseY, scrollDelta);
```

### Configuration

```cpp
// Enable/disable multi-layer scrolling
jsar_multi_layer_scroll::setMultiLayerScrollingEnabled(true);

// Check current state
bool enabled = jsar_multi_layer_scroll::isMultiLayerScrollingEnabled();

// Get active layer count
size_t layerCount = jsar_multi_layer_scroll::getActiveLayerCount();
```

## Implementation Details

### Layer Creation Criteria

A scroll layer is created when:
1. Layout object has CSS `overflow: scroll` or `overflow: auto`
2. Content size is larger than container size
3. No existing layer is associated with the layout object

### Rendering Pipeline

1. **Layer Identification**: During layout, scrollable containers are identified
2. **Layer Creation**: New layers are created for scrollable containers
3. **Depth Ordering**: Layers are sorted by depth (DOM tree depth + z-index)
4. **Independent Rendering**: Each layer renders to its own surface
5. **Compositing**: Layers are composited in correct depth order

### Scroll Event Routing

1. Input system detects scroll event at (x, y)
2. System finds topmost visible layer containing the point
3. Scroll delta is applied to that layer's ScrollableArea
4. Layer is marked dirty for re-rendering

## File Structure

```
src/client/scroll/
├── scrollable_area.hpp/cpp          # Enhanced scrollable area (existing)
├── scroll_layer.hpp/cpp             # Individual layer representation
├── layer_manager.hpp/cpp            # Layer management
└── multi_layer_scroll_api.hpp/cpp   # Integration API

src/client/builtin_scene/
├── multi_layer_renderer.hpp/cpp     # ECS rendering integration
└── web_content.hpp                  # Updated plugin (modified)

tests/client/
└── multi_layer_scroll_tests.cpp     # Unit tests

examples/
└── multi_layer_scroll_example.cpp   # Usage example
```

## Benefits

### Performance
- **Reduced Rendering**: Only dirty layers re-render on scroll
- **GPU Optimization**: Better texture management and compositing
- **Overdraw Reduction**: Each layer only renders its content

### User Experience
- **Smooth Scrolling**: Independent scroll areas don't interfere
- **Proper Layering**: Correct visual depth ordering
- **Responsive Input**: Fast scroll event routing

### Maintainability
- **Minimal Changes**: Integrates with existing systems
- **Backward Compatible**: Falls back to single-layer when disabled
- **Modular Design**: Clean separation of concerns

## Testing

Run the unit tests to verify functionality:

```bash
# Run C++ tests (if build system supports it)
make test

# Run the example
examples/multi_layer_scroll_example
```

## Future Enhancements

1. **CSS Integration**: Better integration with CSS overflow detection
2. **Performance Metrics**: Layer rendering performance monitoring
3. **Advanced Compositing**: Support for CSS transforms and opacity
4. **Memory Management**: Automatic layer cleanup and optimization
5. **Touch Events**: Support for touch-based scrolling

## Notes

- This implementation requires GLM for vector math
- Integration with existing layout and rendering systems
- Designed to be minimal and non-intrusive
- Falls back gracefully when disabled