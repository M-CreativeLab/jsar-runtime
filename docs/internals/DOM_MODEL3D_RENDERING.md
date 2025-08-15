# DOM Model3D Rendering Architecture

This document describes the internal architecture for rendering 3D models through the DOM `<model>` element in JSAR Runtime.

## Overview

The JSAR Runtime supports embedding 3D models directly in HTML content through the `<model>` element. The architecture follows a clear separation between DOM interface, layout management, and rendering components.

## Architecture Components

### 1. DOM Layer (`HTMLModelElement`)

The `HTMLModelElement` class provides the DOM API for the `<model>` element:

```cpp
namespace dom {
  class HTMLModelElement : public HTMLElement
}
```

**Responsibilities:**
- Expose HTML attributes (`src`, `type`, `autoplay`, `loading`)
- Handle model loading and parsing
- Interface with layout system
- Event dispatching (load, error)

**Key Methods:**
- `setSrc()`, `setType()`, `setAutoplay()`, `setLoading()` - Attribute management
- `loadModel()` - Initiate model loading from URL
- `detectModelType()` - Auto-detect model format from file extension
- `parseModel()` - Parse loaded model data

### 2. Layout Layer (`LayoutModel3d`)

The `LayoutModel3d` class bridges the DOM and rendering systems:

```cpp
namespace client_layout {
  class LayoutModel3d : public LayoutObject
}
```

**Responsibilities:**
- Integrate with CSS layout system
- Manage model positioning and sizing
- Create ECS entities and components for rendering
- Handle custom geometry rendering path

**Key Features:**
- Inherits from `LayoutObject` for CSS integration
- Calls `enableCustomGeometry()` to bypass standard WebContent rendering
- Creates `GaussianSplattingModel3d` components for 3DGS models
- Manages entity lifecycle (creation/destruction)

### 3. ECS Rendering System

The rendering uses an Entity-Component-System architecture within `builtin_scene`:

#### Components

- **`GaussianSplattingModel3d`**: Stores individual model's splat data
- **`GaussianSplatsMesh`**: Global mesh for scene-wide splat management
- **`GaussianSplattingMaterial`**: Handles rendering shaders and GPU state

#### Systems

- **`GaussianSplatsManagerSystem`**: Collects splats from individual models
- **`RenderSystem`**: Processes transparent rendering pass

## 3D Gaussian Splatting Support

### File Format Support

JSAR supports 3D Gaussian Splatting through:

- **KSplat format** (`.ksplat`): Binary optimized format
- **PLY format** (`.ply`): Standard point cloud format with splat attributes

### Rendering Pipeline

```
HTMLModelElement → LayoutModel3d → GaussianSplattingModel3d → GaussianSplatsMesh → Render
```

1. **Individual Storage**: Each `<model>` element creates a `GaussianSplattingModel3d` component
2. **Global Collection**: `GaussianSplatsManagerSystem` collects all splats into `GaussianSplatsMesh`
3. **Depth Sorting**: Global sorting using WebXR view matrices for proper transparency
4. **Instanced Rendering**: Single draw call for all splats using `GaussianSplattingMaterial`

### Virtual Drawing Pattern

The material system implements a virtual drawing pattern:

```cpp
// Material base class
virtual bool drawMeshImpl(std::shared_ptr<client_graphics::WebGLProgram> program,
                          std::shared_ptr<Mesh3d> mesh) { return false; }

// GaussianSplattingMaterial implementation
bool drawMeshImpl(...) override {
  // Custom instanced rendering logic
  glContext->drawElementsInstanced(/*...*/);
  return true; // Handled custom drawing
}
```

This allows materials to implement custom drawing while maintaining standard Material lifecycle.

## Data Flow

### Model Loading

1. **HTML Parsing**: `<model src="scene.ksplat" type="3dgs">` creates `HTMLModelElement`
2. **Layout Creation**: DOM creates `LayoutModel3d` during layout phase
3. **Model Fetch**: `loadModel()` fetches data asynchronously
4. **Format Detection**: `detectModelType()` determines format from extension/type hint
5. **Parsing**: Format-specific loader extracts splat data
6. **ECS Integration**: `LayoutModel3d` creates `GaussianSplattingModel3d` component

### Rendering Loop

1. **Collection Phase**: `GaussianSplatsManagerSystem` runs before `RenderSystem`
2. **Global Update**: All splats collected into `GaussianSplatsMesh`
3. **Depth Sorting**: Splats sorted by WebXR view matrix distance
4. **Transparent Pass**: `RenderSystem` processes `RenderPass::kTransparents`
5. **Material Rendering**: `GaussianSplattingMaterial::drawMeshImpl()` performs instanced drawing

## Usage Examples

### Basic HTML Usage

```html
<!-- Auto-detected 3DGS model -->
<model src="scene.ksplat"></model>

<!-- Explicit type hint -->
<model src="data.ply" type="3dgs"></model>

<!-- With loading control -->
<model src="large_scene.ksplat" loading="lazy"></model>
```

### Multiple Models

```html
<!-- Multiple 3DGS models rendered together -->
<model src="building.ksplat"></model>
<model src="vegetation.ksplat"></model>
<model src="characters.ksplat"></model>
```

All models are automatically collected and rendered in a single efficient pass with proper depth sorting.

### JavaScript API

```javascript
const model = document.createElement('model');
model.src = 'scene.ksplat';
model.type = '3dgs';
model.addEventListener('load', () => console.log('Loaded'));
document.body.appendChild(model);
```

## Performance Considerations

### Efficient Multi-Model Rendering

- **Global Batching**: All 3DGS models in a scene rendered in single draw call
- **Instanced Rendering**: Uses `drawElementsInstanced()` for optimal GPU utilization
- **Depth Sorting**: Background sorting prevents main thread blocking

### WebXR Integration

- **View Matrix Sorting**: Uses WebXR viewer pose for accurate depth sorting
- **Multi-view Support**: Handles both XR and traditional rendering pipelines
- **Fallback Behavior**: Uses camera transform when XR not available

### Memory Management

- **Component-based Storage**: Individual models store their own splat data
- **Global Mesh**: Single mesh instance handles scene-wide rendering
- **Resource Cleanup**: Proper entity/component lifecycle management

## Future Enhancements

- **Streaming Support**: Large dataset progressive loading
- **Level of Detail**: Distance-based quality adaptation  
- **Animation**: Temporal splat data support
- **Culling**: Frustum and occlusion optimizations
- **Compression**: Web-optimized format variants

## Integration with Standard Web Technologies

The 3D model rendering integrates seamlessly with standard web technologies:

- **CSS Layout**: Models participate in standard CSS layout (positioning, transforms)
- **DOM Events**: Standard load/error events for model lifecycle
- **WebXR**: Native integration with WebXR rendering pipeline
- **Accessibility**: Future support for spatial accessibility features

This architecture ensures that 3D models feel like natural web content while providing high-performance rendering capabilities.