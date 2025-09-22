# Ray Renderer Usage Example

The ray renderer provides visualization of input rays and collision cursors for XR applications. Here's how to use it:

## Basic Usage

The ray renderer is automatically integrated into the main renderer and runs during the transparents render pass. It visualizes rays from all active input sources (gaze, controllers, hands).

### Accessing the Ray Renderer

```cpp
#include <renderer/renderer.hpp>

// Get the main renderer instance
auto& renderer = renderer::TrRenderer::GetRendererRef();

// Get the ray renderer
auto* rayRenderer = renderer.getRayRenderer();
```

### Configuring Ray Appearance

```cpp
// Configure global ray appearance
renderer::RayConfig rayConfig;
rayConfig.color = glm::vec4(0.0f, 1.0f, 0.0f, 0.8f); // Green with transparency
rayConfig.width = 2.0f;                                // 2 pixel width
rayConfig.maxLength = 10.0f;                           // 10 meter max length
rayConfig.showRay = true;                              // Enable ray visualization

rayRenderer->setGlobalRayConfig(rayConfig);
```

### Configuring Cursor Appearance

```cpp
// Configure global cursor appearance
renderer::CursorConfig cursorConfig;
cursorConfig.size = 0.02f;                                      // 2cm cursor size
cursorConfig.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);       // White cursor
cursorConfig.showCursor = true;                                 // Enable cursor visualization
cursorConfig.texturePath = "assets/cursors/default.png";       // Optional texture

rayRenderer->setGlobalCursorConfig(cursorConfig);
```

### Per-Input Source Configuration

```cpp
// Configure appearance for specific input sources
int gazeInputSourceId = 1;
renderer::RayConfig gazeRayConfig;
gazeRayConfig.color = glm::vec4(1.0f, 1.0f, 0.0f, 0.6f); // Yellow gaze ray
rayRenderer->setRayConfig(gazeInputSourceId, gazeRayConfig);

renderer::CursorConfig gazeCursorConfig;
gazeCursorConfig.size = 0.01f; // Smaller cursor for gaze
rayRenderer->setCursorConfig(gazeInputSourceId, gazeCursorConfig);
```

### Enabling/Disabling Visualization

```cpp
// Enable or disable globally
renderer.setRayVisualizationEnabled(true);
renderer.setCursorVisualizationEnabled(true);

// Or through the ray renderer directly
rayRenderer->setRayVisualizationEnabled(false);
rayRenderer->setCursorVisualizationEnabled(false);
```

## How It Works

1. **Input Source Integration**: The ray renderer automatically fetches rays from all active input sources:
   - Gaze input (eye tracking)
   - Main controller
   - Hand tracking (left and right hands)
   - Screen controllers

2. **Depth Buffer Intersection**: For each ray, the renderer:
   - Samples points along the ray direction
   - Checks against the depth buffer to find intersections
   - Places cursors at intersection points

3. **Rendering Pipeline**: The visualization happens during the transparents render pass:
   - Rays are rendered as 3D lines
   - Cursors are rendered as billboarded quads
   - Proper depth testing and blending is applied

## Technical Details

### Ray Visualization
- Uses OpenGL ES 3.0 compatible shaders
- Renders rays as GL_LINES with configurable width
- Supports transparency through alpha blending

### Cursor Visualization
- Renders as billboarded quads that always face the camera
- Supports both solid colors and textured cursors
- Positioned at depth buffer intersection points

### Performance Considerations
- Ray-depth intersection uses sampling (20 samples per ray by default)
- Minimal OpenGL state changes
- Efficient vertex buffer updates for dynamic rays

## Integration Points

The ray renderer is automatically called during the rendering pipeline:

```cpp
// In TrRenderer::onTransparentsRenderPass()
if (rayRenderer != nullptr) {
    // Update ray visualizations from input sources
    rayRenderer->updateRays(constellation->xrDevice);
    
    // Render rays and cursors
    rayRenderer->render(viewMatrix, projMatrix, framebufferId, 
                        viewport.width, viewport.height);
}
```

This ensures rays are rendered after all opaque and transparent geometry, providing accurate depth-based cursor placement.