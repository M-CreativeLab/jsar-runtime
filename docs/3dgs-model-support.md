# 3D Model Support in JSAR

JSAR now supports 3D model rendering through the `<model>` HTML element, including initial support for 3D Gaussian Splatting (3DGS) models.

## HTML Model Element

The `<model>` element provides a way to embed 3D models directly in your HTML content.

### Basic Usage

```html
<!-- Load a GLTF model -->
<model src="path/to/model.gltf"></model>

<!-- Load a 3DGS model -->
<model src="path/to/model.gsplat" type="3dgs"></model>

<!-- Load a PLY file containing 3DGS data -->
<model src="path/to/model.ply" type="3dgs"></model>
```

### Attributes

- **`src`** (string): The URL or path to the 3D model file
- **`type`** (string, optional): Hint for the model type. Supported values:
  - `"gltf"` - GLTF/GLB models (default for .gltf/.glb files)
  - `"3dgs"` or `"gaussian-splatting"` - 3D Gaussian Splatting models
  - If not specified, type is auto-detected from file extension
- **`autoplay`** (boolean): Whether to automatically play animations (default: false)
- **`loading`** (string): Loading behavior. Values: `"auto"`, `"lazy"`, `"eager"` (default: "auto")

### JavaScript API

```javascript
// Create a model element
const model = document.createElement('model');

// Set properties
model.src = 'path/to/model.gsplat';
model.type = '3dgs';
model.autoplay = true;
model.loading = 'eager';

// Add to DOM
document.body.appendChild(model);

// Event handling (future implementation)
model.addEventListener('load', () => {
  console.log('Model loaded successfully');
});

model.addEventListener('error', (event) => {
  console.error('Failed to load model:', event.detail);
});
```

## 3D Gaussian Splatting (3DGS) Support

### Overview

3D Gaussian Splatting is a novel rendering technique that represents 3D scenes as collections of 3D Gaussian ellipsoids. JSAR provides initial support for 3DGS models with the following features:

- **File Format Support**: `.gsplat` and `.ply` files
- **Concurrent Sorting**: Gaussian ellipsoids are sorted in background threads
- **Proper Blending**: 3DGS rendering occurs after the transparent pass for correct compositing
- **Global Rendering**: Multiple `<model>` elements with 3DGS content are rendered together for optimal performance

### Supported File Formats

#### .gsplat Files
Binary format optimized for 3D Gaussian Splatting data.

#### .ply Files
PLY (Polygon File Format) files containing 3DGS data with the required vertex attributes:
- Position (x, y, z)
- Color (r, g, b)
- Opacity (alpha)
- Scale (scale_x, scale_y, scale_z)
- Rotation (quaternion: rot_x, rot_y, rot_z, rot_w)

### Rendering Pipeline Integration

The 3DGS renderer is integrated into JSAR's rendering pipeline as follows:

1. **Opaque Pass**: Regular opaque geometry
2. **Transparent Pass**: Transparent objects
3. **3DGS Pass**: All 3DGS models rendered together with proper depth sorting

### Performance Considerations

- **Concurrent Sorting**: Gaussian ellipsoids are sorted by depth using background threads to avoid blocking the main rendering thread
- **Global Batching**: When multiple `<model>` elements contain 3DGS data, all ellipsoids are collected and rendered in a single pass
- **WebGL2 Backend**: 3DGS rendering uses WebGL2 for optimal performance

## Architecture

### Components

1. **HTMLModelElement**: DOM element implementation
2. **TrGaussianSplattingRenderer**: Core 3DGS rendering engine
3. **Render Pipeline Integration**: Hooks into the existing transparent render pass

### Current Implementation Status

✅ **Implemented:**
- `<model>` HTML element with full DOM API
- Basic 3DGS renderer structure
- Render pipeline integration
- File format detection
- Concurrent sorting infrastructure

🚧 **Planned/TODO:**
- Complete 3DGS file format parsers (.gsplat, .ply)
- WebGL2 shader implementation for Gaussian rendering
- Background sorting thread implementation
- Integration with existing GLTF model loading
- Event system (load, error events)
- Performance optimizations

### Example Usage in XTML

```xml
<xtml>
  <head>
    <title>3DGS Model Viewer</title>
  </head>
  <space>
    <!-- Traditional GLTF model -->
    <model src="assets/scene.gltf" autoplay="true"></model>
    
    <!-- 3DGS model -->
    <model src="assets/gaussian_scene.gsplat" type="3dgs"></model>
    
    <!-- Multiple 3DGS models will be rendered together -->
    <model src="assets/object1.ply" type="3dgs"></model>
    <model src="assets/object2.ply" type="3dgs"></model>
  </space>
  <script>
    spatialDocument.addEventListener('spaceReady', () => {
      console.log('Space ready with 3D models');
    });
  </script>
</xtml>
```

## Future Enhancements

- **Streaming**: Support for streaming large 3DGS datasets
- **Level of Detail (LOD)**: Adaptive quality based on distance and performance
- **Culling**: Frustum and occlusion culling for large scenes
- **Compression**: Optimized file formats for web delivery
- **Lighting Integration**: Integration with JSAR's lighting system
- **Animation**: Support for animated 3DGS content