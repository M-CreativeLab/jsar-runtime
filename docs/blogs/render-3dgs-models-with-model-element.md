# Render Gaussian Splatting Models with `<model>` Element

*Published: August 2025*

The world of 3D rendering has been revolutionized by 3D Gaussian Splatting (3DGS), a breakthrough technique that enables photorealistic real-time rendering of complex 3D scenes. JSAR Runtime takes this innovation further by bringing 3DGS directly into the web browser through native HTML support, making it as easy to embed a 3D Gaussian Splat model as it is to add an image to your webpage.

## What is 3D Gaussian Splatting?

3D Gaussian Splatting represents 3D scenes as collections of 3D Gaussian primitives (splats), each with properties like position, color, opacity, scale, and rotation. Unlike traditional mesh-based rendering, 3DGS can capture and reproduce incredibly detailed and realistic scenes with complex lighting effects, transparency, and fine surface details that would be computationally prohibitive with conventional approaches.

The technique has gained massive adoption in computer vision and 3D reconstruction because it can:
- Render photorealistic scenes in real-time
- Handle complex transparency and volumetric effects naturally
- Capture fine details like hair, foliage, and translucent materials
- Provide smooth performance even with millions of splats

## HTML Integration: Making 3DGS Web-Native

<video src="http://ar.rokidcdn.com/web-assets/pages/record-3dgs-model.mov" controls></video>

JSAR Runtime implements support for the HTML `<model>` element, enabling direct rendering of 3DGS models in HTML. This implementation allows 3DGS models to be treated as first-class citizens in web content, making them as easy to work with as images or videos.

### Basic HTML Usage

The simplest way to embed a 3DGS model is through the `<model>` element:

```html
<!DOCTYPE html>
<html>
<head>
  <title>3DGS in HTML</title>
</head>
<body>
  <!-- Auto-detected 3DGS model -->
  <model src="assets/scene.ksplat"></model>

  <!-- With loading control -->
  <model src="assets/large_scene.ksplat" loading="progressive"></model>
</body>
</html>
```

### File Format Support

JSAR supports three primary formats for 3DGS models:

#### KSplat Format (`.ksplat`) - Recommended
- **Binary optimized format** designed specifically for web delivery
- **Compressed data** with efficient storage of position, scale, rotation, and color
- **Fast loading** with minimal parsing overhead
- **Streamable** for large datasets

#### SPZ Format (`.spz`) - Ultra Compressed
- **Compressed splat format** with ultra-high compression ratio
- **Optimized for mobile** and streaming applications
- **Minimal bandwidth** requirements for network delivery
- **Enhanced loading performance** with advanced compression algorithms

#### PLY Format (`.ply`)
- **Standard point cloud format** with 3DGS extensions
- **Human-readable** (in ASCII variant) for debugging
- **Widely supported** by 3DGS creation tools
- **Larger file sizes** compared to KSplat and SPZ

```html
<!-- Production: Use KSplat for optimal performance -->
<model src="production_scene.ksplat"></model>

<!-- Mobile/Streaming: Use SPZ for ultra compression -->
<model src="mobile_scene.spz"></model>

<!-- Development: Use PLY for easier debugging -->
<model src="debug_scene.ply"></model>
```

### Advanced HTML Integration

While these features are still under development, let's explore how the `<model>` element could seamlessly integrate with standard web technologies in the future:

#### CSS Layout Integration
```html
<style>
.scene-container {
  width: 800px;
  height: 600px;
  position: relative;
  border: 2px solid #333;
  border-radius: 8px;
  overflow: hidden;
}

.main-scene {
  width: 100%;
  height: 100%;
  transform: rotateY(15deg);
}

.floating-model {
  position: absolute;
  top: 50px;
  right: 50px;
  width: 200px;
  height: 150px;
  transform: scale(0.5) rotateX(-10deg);
}
</style>

<div class="scene-container">
  <model src="main_scene.ksplat" class="main-scene"></model>
  <model src="detail_object.ksplat" class="floating-model"></model>
</div>
```

#### JavaScript API
```javascript
// Create models dynamically
const model = document.createElement('model');
model.src = 'dynamic_scene.ksplat';
model.type = '3dgs';

// Event handling
model.addEventListener('load', () => {
  console.log('3DGS model loaded successfully');
  updateScene();
});

model.addEventListener('error', (e) => {
  console.error('Failed to load 3DGS model:', e);
});

// Add to document
document.body.appendChild(model);

// Property access
console.log(`Model source: ${model.src}`);
console.log(`Loading complete: ${model.complete}`);
```

## Technical Implementation Deep Dive

### Architecture Overview

JSAR's 3DGS implementation follows a sophisticated multi-layer architecture:

```
┌─────────────────────────────────────────┐
│             HTML DOM Layer               │  ← HTMLModelElement
├─────────────────────────────────────────┤
│           CSS Layout System             │  ← LayoutModel3d  
├─────────────────────────────────────────┤
│      Entity-Component-System (ECS)      │  ← GaussianSplattingModel3d
├─────────────────────────────────────────┤
│          Rendering Pipeline             │  ← GaussianSplatsMesh
├─────────────────────────────────────────┤
│         GPU Shader System               │  ← GaussianSplattingMaterial
└─────────────────────────────────────────┘
```

1. **HTMLModelElement**: Provides the DOM interface, handles file loading and parsing
2. **LayoutModel3d**: Integrates with CSS layout and manages the transition to 3D rendering
3. **GaussianSplattingModel3d**: ECS component storing individual model's splat data
4. **GaussianSplatsMesh**: Global mesh system for efficient multi-model rendering
5. **GaussianSplattingMaterial**: Custom material with optimized shaders for 3DGS rendering

### ECS (Entity-Component-System) Architecture

The rendering system uses a high-performance ECS architecture optimized for spatial computing:

#### Components
- **`GaussianSplattingModel3d`**: Stores individual model splat data and metadata
- **`GaussianSplatsMesh`**: Global mesh managing scene-wide splat collection
- **`GaussianSplattingMaterial`**: Shader material handling GPU rendering state

#### Systems
- **`GaussianSplatsManagerSystem`**: Collects splats from all models in the scene
- **`RenderSystem`**: Executes rendering passes with proper depth sorting

### Data Flow Pipeline

```
HTML Parse → DOM Creation → Layout → ECS Entity → Global Collection → GPU Rendering
     ↓              ↓           ↓         ↓              ↓              ↓
<model src=...> → HTMLModelElement → LayoutModel3d → GaussianSplattingModel3d → GaussianSplatsMesh → GPU
```

1. **HTML Parsing**: Browser encounters `<model>` element, creates HTMLModelElement
2. **Model Loading**: Asynchronous fetch and format detection
3. **Layout Integration**: LayoutModel3d integrates model with CSS layout system
4. **ECS Creation**: GaussianSplattingModel3d component stores parsed splat data
5. **Global Collection**: GaussianSplatsManagerSystem collects all splats
6. **Rendering**: Single instanced draw call renders all splats efficiently

## Shader Optimization: The Performance Engine

The heart of JSAR's 3DGS performance lies in heavily optimized shaders that push the boundaries of what's possible in real-time web rendering.

### Specific Pipeline for 3DGS Rendering

In JSAR's 3DGS rendering implementation, we chose to store splats data in textures rather than traditional instance buffers, a design decision primarily based on performance optimization considerations.

One of the core challenges in 3DGS rendering is the need for depth sorting of splats to ensure correct transparency blending. If instance buffers were used to store all splat data (including position, scale, rotation, color, etc.), each view change or camera movement would require re-sorting and transferring large amounts of splat data to the GPU, leading to:

- **Massive Data Transfer**: Each splat contains multiple attributes (center, scale, quat, rgba), resulting in enormous data volumes
- **Frequent GPU/CPU Synchronization**: Sorting operations require CPU involvement, increasing synchronization overhead
- **Memory Bandwidth Bottleneck**: Repeated transfer of large data volumes consumes precious memory bandwidth

Therefore, JSAR adopts a hybrid storage strategy:

1. **Texture Storage for Base Data**: All splat base attributes (position, scale, rotation, color) are stored in GPU textures, remaining constant during rendering
2. **Instance Buffer for Indices**: Only sorted splat indices are passed through the instance buffer, significantly reducing data volume
3. **Shader Index Access**: The vertex shader uses indices to fetch corresponding splat data via `texelFetch` from textures

The advantages of this design include:
- Only lightweight index data needs synchronization for each sort
- Over 90% reduction in data transfer volume
- Minimized GPU/CPU synchronization overhead
- Efficient utilization of GPU texture cache characteristics

Through this optimization, JSAR can significantly improve rendering performance for large-scale 3DGS scenes while maintaining rendering quality.

### Data Compression Strategy

With the aforementioned prerequisites in place, we need to optimize the number of `texelFetch` calls when rendering each splat, therefore JSAR adopts [Sparkjs][]'s aggressive compression scheme to maximize GPU efficiency.

This optimized compression design enables each splat to be rendered with just a single `texelFetch` operation, loading all essential data (center position, scale, quaternion rotation, and RGBA color) in one GPU memory access. This unified data access pattern significantly reduces memory bandwidth requirements and improves rendering performance.

#### Position Compression (Half-Float)
```glsl
// Positions stored as half-floats (16-bit) instead of full floats (32-bit)
vec3 decompressPositionHalf(uint word0, uint word1) {
  uint hx = word0 & 0xFFFFu;
  uint hy = (word0 >> 16u) & 0xFFFFu; 
  uint hz = word1 & 0xFFFFu;
  
  return vec3(
    unpackHalf(hx),
    unpackHalf(hy), 
    unpackHalf(hz)
  );
}
```
**Memory Savings**: 50% reduction in position data (12 bytes → 6 bytes per splat)

#### Scale Compression (8-bit Logarithmic)
```glsl
// Scales stored in logarithmic space with 8-bit precision
vec3 decompressScaleLog(uint word2) {
  uint ix = word2 & 0xFFu;
  uint iy = (word2 >> 8u) & 0xFFu;
  uint iz = (word2 >> 16u) & 0xFFu;
  
  float nx = float(ix) / 255.0;
  float ny = float(iy) / 255.0;
  float nz = float(iz) / 255.0;
  
  return vec3(
    exp2(scaleMin.x + nx * (scaleMax.x - scaleMin.x)),
    exp2(scaleMin.y + ny * (scaleMax.y - scaleMin.y)),
    exp2(scaleMin.z + nz * (scaleMax.z - scaleMin.z))
  );
}
```
**Memory Savings**: 75% reduction in scale data (12 bytes → 3 bytes per splat)

#### Quaternion Compression (Octahedral Mapping)
```glsl
// Quaternions compressed using octahedral mapping (24-bit)
vec4 decompressQuaternionOct(uint word1, uint word2) {
  uint uQuat = ((word1 >> 16u) & 0xFFFFu) | ((word2 >> 8u) & 0xFF0000u);
  return decodeQuatOctXy88R8(uQuat);
}
```
**Memory Savings**: 40% reduction in rotation data (16 bytes → 3 bytes per splat)

### GPU Memory Layout

Each splat is packed into exactly 16 bytes (4 32-bit words):

```
Word 0: [pos.x half] [pos.y half]           (32-bit)
Word 1: [pos.z half] [quat high 16-bit]     (32-bit)  
Word 2: [quat low 8] [scale.x] [scale.y] [scale.z] (32-bit)
Word 3: [color.r] [color.g] [color.b] [color.a]    (32-bit)
```

This ultra-compact representation enables:
- **4x memory efficiency** compared to uncompressed data
- **Better cache utilization** on GPU
- **Reduced bandwidth** requirements for streaming
- **Higher splat counts** within GPU memory limits

### Vertex Shader Pipeline

The vertex shader performs several critical optimizations:

#### Efficient Texture Sampling
```glsl
// Power-of-2 texture dimensions enable bit-shift operations
const int TEXTURE_WIDTH_BITS = 10;  // 1024 width
const int TEXTURE_WIDTH = 1 << TEXTURE_WIDTH_BITS;

ivec2 getSplatTexCoord(int index) {
  int x = index & (TEXTURE_WIDTH - 1);  // Modulo via bitwise AND
  int y = index >> TEXTURE_WIDTH_BITS;  // Division via bit shift
  return ivec2(x, y);
}
```

#### Covariance Matrix Computation
```glsl
// Efficient 3D to 2D covariance projection
mat3 computeCov3D(mat4 viewMatrix, vec4 splatQuat, vec3 splatScale) {
  mat3 RS = scaleQuaternionToMat3(splatScale, splatQuat);
  mat3 T = mat3(viewMatrix) * RS;
  return T * transpose(T);
}

// Jacobian computation for splat projection
vec2 focal = 0.5 * renderSize * vec2(projectionMatrix[0][0], projectionMatrix[1][1]);
float invZ = 1.0 / viewCenter.z;
vec2 J1 = focal * invZ;
vec2 J2 = -(J1 * viewCenter.xy) * invZ;
```

#### Early Culling Optimizations
```glsl
// Multiple levels of early culling to avoid unnecessary computation
if (rgba.a < minAlpha) return;                    // Alpha culling
if (viewCenter.z >= 0.0) return;                  // Behind camera culling  
if (abs(clipCenter.z) >= clipCenter.w) return;    // Near/far plane culling
if (abs(clipCenter.x) > clip || abs(clipCenter.y) > clip) return; // Frustum culling
```

### Fragment Shader Optimization

The fragment shader focuses on accurate Gaussian evaluation with performance optimizations:

```glsl
void main() {
  vec4 rgba = vRgba;
  
  // Efficient distance calculation
  float z = dot(vSplatUv, vSplatUv);
  if (z > (maxStdDev * maxStdDev)) {
    discard;
  }
  
  // Optimized Gaussian falloff
  const float falloff = 1.0;
  rgba.a *= mix(1.0, exp(-1.5 * z), falloff);
  
  if (rgba.a < minAlpha) {
    discard;
  }
  
  fragColor = rgba;
}
```

### WebXR Multiview Support

The applications in JSAR of course supports more efficient multiview rendering:

```glsl
#ifdef MULTIVIEW
uniform mat4 viewR;
uniform mat4 projectionR;

// Single-pass stereo rendering
if (VIEW_ID == 0u) {
  viewMatrix = view;
  projectionMatrix = projection;
} else {
  viewMatrix = viewR; 
  projectionMatrix = projectionR;
}
#endif
```

This enables:
- **50% reduction** in draw calls for VR rendering
- **Automatic stereo correction** for spatial computing
- **Optimal performance** on XR devices

## Performance Optimization Strategies

### Global Batching System

Since 3DGS rendering relies on alpha blending, multiple 3DGS models require global depth sorting of all Gaussian splats to ensure correct multi-object rendering (this approach was inspired by Sparkjs, which implements a similar global renderer for collecting 3DGS-specific meshes), thus instead of rendering each `<model>` element separately, JSAR implements a global batching system:

```cpp
// All 3DGS models collected into single draw call
void GaussianSplatsManagerSystem::update() {
  // Collect splats from all HTMLModelElement instances
  collectSplatsFromScene();

  // Update global mesh with all collected splats
  globalMesh->updateInstanceData(allSplats);

  // Perform depth sorting for proper transparency
  sortSplatsByDepth(cameraViewMatrix);
}
```

### Advanced Depth Sorting

Proper transparency rendering requires careful depth sorting:

```cpp
void sortSplatsByDepth(const glm::mat4& viewMatrix) {
  std::sort(splatInstances.begin(), splatInstances.end(),
    [&viewMatrix](const SplatInstance& a, const SplatInstance& b) {
      vec3 posA = vec3(viewMatrix * vec4(a.position, 1.0));
      vec3 posB = vec3(viewMatrix * vec4(b.position, 1.0));
      return posA.z > posB.z; // Back-to-front for transparency
    });
}
```

### Memory Management

JSAR implements intelligent memory management for large scenes:

```cpp
// Component-based storage with efficient cleanup
class GaussianSplattingModel3d : public ecs::Component {
private:
  std::vector<CompressedSplat> splatData_;  // Per-model storage
  bool dataUploaded_ = false;               // GPU upload tracking
  
public:
  ~GaussianSplattingModel3d() {
    // Automatic cleanup when model element removed
    if (dataUploaded_) {
      globalMesh->removeSplats(this);
    }
  }
};
```

### Distance-Based Culling

JSAR implements configurable distance-based culling for performance optimization:

```cpp
// Distance-based culling in vertex shader
void main() {
  // Calculate distance from camera
  vec4 viewCenter = viewMatrix * vec4(center, 1.0);
  float distance = length(viewCenter.xyz);
  
  // Cull splats beyond maxDistance threshold
  if (distance > maxDistance) {
    return; // Skip rendering this splat
  }
  
  // Continue with normal splat processing...
}
```

Features:
- **Configurable threshold**: `maxDistance` parameter (default 1.5m)
- **GPU-based culling**: Efficient vertex shader implementation
- **Performance scaling**: Automatically reduces splat count based on viewing distance
- **Quality preservation**: Maintains visual fidelity for nearby objects

## Future Enhancements and Roadmap

JSAR's 3DGS implementation continues to evolve with exciting upcoming features:

### Streaming Support

**Progressive loading** 

For massive datasets: Implement chunked loading with priority-based scheduling to handle multi-gigabyte 3DGS scenes. This enables immediate rendering of nearby splats while background loading distant regions, dramatically reducing initial load times for large architectural or outdoor scenes.

**Level-of-detail (LOD)**

Based on viewing distance: Automatically reduce splat density and quality for distant objects using hierarchical clustering algorithms. This maintains visual fidelity for close objects while significantly improving performance in complex scenes with millions of splats.

**Adaptive quality**

Based on device capabilities: Dynamically adjust rendering parameters, compression levels, and splat counts based on GPU memory, processing power, and network bandwidth. This ensures optimal performance across devices from mobile phones to high-end VR headsets.

### Advanced Culling

**Frustum culling**

For off-screen splats: Implement GPU-based frustum culling to eliminate splats outside the camera's field of view before rendering. This reduces GPU workload by 30-70% in typical scenes, especially beneficial for wide-angle or panoramic 3DGS captures.

**Occlusion culling**

For hidden geometry: Use depth buffer analysis and spatial partitioning to identify and skip splats occluded by foreground objects. This optimization is particularly effective in indoor scenes with walls and furniture, reducing unnecessary fragment processing.

### Animation Support

**Dyno Support**

Inspired by [Sparkjs][]'s innovative [Dyno][] shading language, JSAR plans to implement a Web-Native version of the [Dyno][] API for advanced 3DGS manipulation and animation.

In Sparkjs, Dyno serves as a powerful language for transforming and generating Gaussian splats dynamically, enabling complex scene modifications and procedural content generation. JSAR's Web-Native implementation will bring these capabilities directly to the browser, allowing developers to programmatically manipulate splat properties, create dynamic transformations, and implement sophisticated animation systems through familiar web APIs.

This approach will enable real-time splat morphing, procedural scene generation, and interactive 3DGS content that responds to user input or environmental changes, all while maintaining the performance and accessibility advantages of native web integration.

### Compression Improvements

**Streaming-friendly**

Data organization: Restructure splat data layout to enable efficient partial downloads and progressive enhancement. This allows users to interact with 3DGS models before full download completion, improving perceived performance in web applications.

**Hardware-specific optimizations**

For different GPU architectures: Implement adaptive shader compilation and memory layout optimization for different GPU vendors (NVIDIA, AMD, Apple Silicon). This ensures optimal performance across diverse hardware configurations in web browsers.

## Acknowledgments

JSAR's 3D Gaussian Splatting implementation builds upon the excellent work of the open-source community. We would like to express our sincere gratitude to:

- **[Sparkjs][]** - Our implementation draws significant inspiration from Sparkjs's innovative approach to web-based 3DGS rendering. Their pioneering work in bringing 3DGS to the browser has been instrumental in shaping our architecture.

- **The broader 3DGS community** - Including the original 3D Gaussian Splatting research team, various open-source renderer implementations, and the countless developers who have contributed to advancing this technology.

- **WebGL and WebXR communities** - For providing the foundational technologies that make real-time 3D rendering in browsers possible.

The collaborative nature of open-source development has been crucial in advancing 3DGS technology from research to practical web applications. We're proud to contribute back to this ecosystem and look forward to continued collaboration with the community.

## Conclusion

JSAR Runtime's integration of 3D Gaussian Splatting into HTML represents a fundamental shift in how we think about 3D content on the web. By treating 3DGS models as first-class HTML elements, JSAR makes photorealistic 3D rendering accessible to every web developer.

The combination of intuitive HTML APIs, aggressive performance optimizations, and seamless WebXR integration creates unprecedented opportunities for immersive web experiences. Whether you're building architectural visualizations, product showcases, or spatial computing applications, JSAR's 3DGS support provides the foundation for next-generation web experiences.

The future of the web is spatial, and with JSAR Runtime, that future is available today through the familiar, accessible technologies that web developers already know and love.

---

[Sparkjs]: https://sparkjs.dev/
[Dyno]: https://sparkjs.dev/docs/dyno-overview/
