# Layered WebContent Rendering

This document describes the layered rendering system implemented for nested scrollable containers and clipping in JSAR Runtime.

## Overview

The layered rendering system ensures correct painter's order and clipping behavior when rendering WebContent elements with nested scrollable containers. It addresses the requirements specified in issue #199.

## Architecture

### Core Components

1. **LayerTypes.hpp/cpp** - Data structures for layer hierarchy
   - `LayerNode`: Represents a single layer with clipping and transform info
   - `RenderCommand`: Encapsulates rendering operations (clip, transform, render)
   - `LayerRenderConfig`: Configuration options for rendering behavior

2. **RenderListBuilder.hpp/cpp** - Command generation from layer tree
   - Builds layer hierarchy from ECS WebContent entities
   - Generates ordered render commands via depth-first traversal
   - Maintains parent-child relationships based on layer indices

3. **ClipStack.hpp/cpp** - Clipping state management
   - Manages scissor and stencil clipping operations
   - Optimizes for axis-aligned rectangles when possible
   - Maintains clip region stack with proper push/pop semantics

4. **DebugOverlay.hpp/cpp** - Visualization for debugging
   - Shows layer bounds and hierarchy connections
   - Color-codes layers by type (scrollable, clipped, etc.)
   - Displays scroll indicators and overflow status

5. **LayerRenderer.hpp/cpp** - Main orchestration system
   - Integrates all components into the ECS rendering pipeline
   - Executes render commands in correct order
   - Coordinates with existing WebContent rendering systems

## Layer Hierarchy

### Layer Index Assignment

Layers are assigned indices based on their nesting depth:
- **Layer 0**: Root/outermost containers 
- **Layer 1**: First level of scrollable containers
- **Layer 2**: Nested content within scrollable areas
- **Layer N**: Arbitrarily deep nesting supported

### Rendering Order

The system follows strict painter's order:
1. Outer layers render first (lower layer indices)
2. Inner layers render over outer layers (higher layer indices)
3. Within the same layer, z-index and translateZ determine order

## Render Command Generation

### Algorithm

The `RenderListBuilder` uses depth-first traversal:

```cpp
void traverseLayer(LayerNode* node, vector<RenderCommand>& commands) {
    // 1. Push transform (scroll offset)
    if (node->hasScrollOffset()) {
        commands.push_back(PushTransform{node->scrollOffset});
    }
    
    // 2. Push clip region
    if (node->needsClipping()) {
        commands.push_back(PushClip{node->clipRect});
    }
    
    // 3. Render layer content
    commands.push_back(RenderWebContent{node->entityId});
    
    // 4. Recursively render children
    for (auto& child : node->children) {
        traverseLayer(child, commands);
    }
    
    // 5. Pop clip and transform (reverse order)
    if (node->needsClipping()) {
        commands.push_back(PopClip{});
    }
    if (node->hasScrollOffset()) {
        commands.push_back(PopTransform{});
    }
}
```

### Example Command Sequence

For a 3-layer hierarchy (outer container → scrollable area → inner content):

```
1. PushClip (Layer 0: 400x300)      → Clip to outer container
2. RenderWebContent (Layer 0)       → Draw outer container background
3. PushTransform (Layer 1: -20,-30) → Apply scroll offset
4. PushClip (Layer 1: 200x150)      → Clip to scrollable area
5. RenderWebContent (Layer 1)       → Draw scrollable container
6. PushClip (Layer 2: 180x100)      → Clip to inner content
7. RenderWebContent (Layer 2)       → Draw inner content
8. PopClip (Layer 2)                → Restore previous clip
9. PopClip (Layer 1)                → Restore previous clip  
10. PopTransform (Layer 1)          → Restore previous transform
11. PopClip (Layer 0)               → Restore original clip
```

## Clipping Strategies

### Scissor Test (Preferred)

Used for axis-aligned rectangular clips:
- Hardware-accelerated on most GPUs
- No impact on stencil buffer
- Efficient for simple rectangular bounds

### Stencil Test (Fallback)

Used for complex shapes:
- Rounded rectangles with significant corner radii
- Arbitrary path-based clipping
- Text-based background clipping

### Selection Logic

```cpp
bool shouldUseScissor(const SkRRect& roundedRect) {
    const float maxRadiusForScissor = 2.0f * devicePixelRatio;
    
    for (int i = 0; i < 4; i++) {
        SkVector radius = roundedRect.radii(i);
        if (radius.x() > maxRadiusForScissor || radius.y() > maxRadiusForScissor) {
            return false; // Use stencil for significant corner radii
        }
    }
    return true; // Use scissor for small/no corner radii
}
```

## Integration with Existing Systems

### ECS Pipeline Integration

The layered rendering system integrates with the existing ECS rendering pipeline:

```cpp
// Original pipeline:
RenderBackgroundSystem → RenderImageSystem → RenderTextSystem → UpdateTextureSystem

// New layered pipeline:
LayeredWebContentRenderSystem → RenderBackgroundSystem → RenderImageSystem → RenderTextSystem → UpdateTextureSystem
```

### WebContent Compatibility

- Existing `WebContent` components work without modification
- Layer indices are assigned automatically based on hierarchy
- Individual rendering systems (background, text, image) unchanged
- Texture management remains the same

## Configuration

### Runtime Configuration

```cpp
// Configure layered rendering
WebContentPlugin plugin;
LayerRenderConfig config;
config.enableDebugOverlay = true;
config.preferScissorClipping = true;
config.devicePixelRatio = 2.0f;
plugin.configureLayeredRendering(config);

// Enable debug visualization
plugin.setDebugOverlayEnabled(true);
```

### Debug Overlay

When enabled, the debug overlay shows:
- Layer bounds with color coding
- Hierarchy connections between layers
- Scroll offset indicators
- Overflow status markers
- Layer index labels

## Performance Considerations

### Optimizations

1. **Command Caching**: Render commands are cached and only rebuilt when layer tree changes
2. **Scissor Preference**: Axis-aligned clips use hardware scissor test when possible
3. **Dirty Checking**: Only processes entities that need re-rendering
4. **Minimal State Changes**: Groups operations to reduce GPU state transitions

### Performance Characteristics

- **Setup Cost**: O(n log n) for layer tree construction (where n = number of WebContent entities)
- **Render Cost**: O(c) where c = number of render commands (typically 2-3x number of layers)
- **Memory**: Minimal overhead - only stores layer hierarchy and cached commands

## Testing

### Unit Tests

Located in `tests/client/builtin_scene_layers_tests.cpp`:
- LayerNode hierarchy construction
- Render command generation
- Scroll offset accumulation
- Clipping state management

### Integration Test

Standalone validation in `/tmp/layer_test.cpp` proves the algorithm generates correct command sequences for nested scrollable containers.

### Test Fixture

HTML fixture in `fixtures/nested_scrollable_layers.html` demonstrates the visual requirements and expected behavior.

## Debugging

### Debug Overlay

Enable debug visualization:
```cpp
webContentPlugin.setDebugOverlayEnabled(true);
```

Color coding:
- **Blue**: Scrollable containers with overflow
- **Cyan**: Scrollable containers without overflow  
- **Green**: Layers with clipping enabled
- **Gray**: Basic layers without special properties

### Logging

Add debug output for render command generation:
```cpp
#define LAYER_DEBUG_LOGGING 1
// Enables detailed logging of layer tree construction and command generation
```

## Limitations and Future Work

### Current Limitations

1. **2D Focus**: Optimized for 2D web content; 3D transforms need enhancement
2. **Stencil Complexity**: Complex stencil operations may impact performance
3. **Platform Dependencies**: Some optimizations are GPU-specific

### Future Enhancements

1. **GPU Command Lists**: Generate render commands on GPU for large layer trees
2. **Rounded Corner Optimization**: Hardware-accelerated rounded rectangle clipping
3. **Scroll-Linked Effects**: Support for scroll-driven animations
4. **Sticky Positioning**: Enhanced support for CSS sticky positioning

## API Reference

See individual header files for detailed API documentation:
- `LayerTypes.hpp` - Core data structures
- `RenderListBuilder.hpp` - Command generation
- `ClipStack.hpp` - Clipping state management
- `LayerRenderer.hpp` - Main rendering system
- `DebugOverlay.hpp` - Debug visualization