# Bringing 3D Gaussian Splatting to the Web: A Complete Guide to Using 3DGS Models in HTML

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

JSAR Runtime introduces the `<model>` element, bringing 3DGS models directly into HTML as first-class citizens. This isn't just a WebGL library wrapper – it's a fundamental extension of the web platform that treats 3D models with the same ease and familiarity as images or videos.

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

    <!-- Explicit type specification -->
    <model src="assets/photogrammetry.ply" type="3dgs"></model>

    <!-- With loading control -->
    <model src="assets/large_scene.ksplat" loading="lazy"></model>
</body>
</html>
```

### File Format Support

JSAR supports two primary formats for 3DGS models:

#### KSplat Format (`.ksplat`) - Recommended
- **Binary optimized format** designed specifically for web delivery
- **Compressed data** with efficient storage of position, scale, rotation, and color
- **Fast loading** with minimal parsing overhead
- **Streamable** for large datasets

#### PLY Format (`.ply`)
- **Standard point cloud format** with 3DGS extensions
- **Human-readable** (in ASCII variant) for debugging
- **Widely supported** by 3DGS creation tools
- **Larger file sizes** compared to KSplat

```html
<!-- Production: Use KSplat for optimal performance -->
<model src="production_scene.ksplat"></model>

<!-- Development: Use PLY for easier debugging -->
<model src="debug_scene.ply" type="3dgs"></model>
```

### Advanced HTML Integration

The `<model>` element integrates seamlessly with standard web technologies:

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

#### Multiple Models and Scene Composition
```html
<!-- Complex scene with multiple 3DGS models -->
<div class="architectural-scene">
    <!-- Main building structure -->
    <model src="building_exterior.ksplat" class="building"></model>
    
    <!-- Environmental elements -->
    <model src="trees_vegetation.ksplat" class="environment"></model>
    
    <!-- Dynamic elements -->
    <model src="people_cars.ksplat" class="dynamic-objects"></model>
    
    <!-- Atmospheric effects -->
    <model src="fog_particles.ksplat" class="atmosphere"></model>
</div>
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

### Data Compression Strategy

JSAR uses an aggressive compression scheme to maximize GPU memory efficiency:

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

For VR/AR applications, JSAR supports efficient multiview rendering:

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

Instead of rendering each `<model>` element separately, JSAR implements a sophisticated global batching system:

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

Benefits:
- **Dramatic draw call reduction**: 100+ models → 1 draw call
- **Optimal GPU utilization**: Maximum use of instanced rendering
- **Improved batching efficiency**: Fewer state changes

### Advanced Depth Sorting

Proper transparency rendering requires careful depth sorting:

```cpp
// WebXR-aware depth sorting
void sortSplatsByDepth(const glm::mat4& viewMatrix) {
    std::sort(splatInstances.begin(), splatInstances.end(),
        [&viewMatrix](const SplatInstance& a, const SplatInstance& b) {
            vec3 posA = vec3(viewMatrix * vec4(a.position, 1.0));
            vec3 posB = vec3(viewMatrix * vec4(b.position, 1.0));
            return posA.z > posB.z; // Back-to-front for transparency
        });
}
```

Features:
- **WebXR pose integration**: Uses actual VR headset position
- **Background processing**: Sorting happens off main thread
- **Fallback support**: Camera transform when XR unavailable

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

## Performance Best Practices

### File Format Selection
```html
<!-- ✅ Recommended: Use KSplat for production -->
<model src="scene.ksplat"></model>

<!-- ❌ Avoid: Large PLY files in production -->
<model src="huge_scene.ply" type="3dgs"></model>
```

### Loading Strategies
```html
<!-- ✅ Use lazy loading for non-critical models -->
<model src="background_detail.ksplat" loading="lazy"></model>

<!-- ✅ Preload critical models -->
<model src="main_character.ksplat" loading="eager"></model>
```

### CSS Optimization
```css
/* ✅ Use transform3d to enable hardware acceleration */
.model-container {
    transform: translate3d(0, 0, 0);
    will-change: transform;
}

/* ✅ Avoid frequent reflows with absolute positioning */
.floating-model {
    position: absolute;
    contain: layout style paint;
}
```

### JavaScript Performance
```javascript
// ✅ Batch DOM operations
const models = ['scene1.ksplat', 'scene2.ksplat', 'scene3.ksplat'];
const fragment = document.createDocumentFragment();

models.forEach(src => {
    const model = document.createElement('model');
    model.src = src;
    fragment.appendChild(model);
});

document.body.appendChild(fragment); // Single DOM operation

// ✅ Use intersection observer for viewport culling
const observer = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
        const model = entry.target;
        if (!entry.isIntersecting) {
            model.style.display = 'none'; // Cull off-screen models
        }
    });
});
```

## Advanced Use Cases

### Dynamic Scene Composition

```javascript
class DynamicSceneManager {
    constructor(container) {
        this.container = container;
        this.loadedModels = new Map();
    }
    
    async loadModelSet(sceneName, modelPaths) {
        const loadPromises = modelPaths.map(async (path) => {
            const model = document.createElement('model');
            model.src = path;
            
            return new Promise((resolve, reject) => {
                model.addEventListener('load', () => resolve(model));
                model.addEventListener('error', reject);
            });
        });
        
        const models = await Promise.all(loadPromises);
        this.loadedModels.set(sceneName, models);
        
        // Add all models to scene
        models.forEach(model => this.container.appendChild(model));
    }
    
    switchScene(sceneName) {
        // Hide all current models
        this.container.querySelectorAll('model').forEach(model => {
            model.style.display = 'none';
        });
        
        // Show requested scene models
        const sceneModels = this.loadedModels.get(sceneName) || [];
        sceneModels.forEach(model => {
            model.style.display = 'block';
        });
    }
}

// Usage
const sceneManager = new DynamicSceneManager(document.querySelector('.scene-container'));
await sceneManager.loadModelSet('indoor', ['room.ksplat', 'furniture.ksplat']);
await sceneManager.loadModelSet('outdoor', ['landscape.ksplat', 'trees.ksplat']);

sceneManager.switchScene('indoor');
```

### Integration with WebXR

```javascript
// WebXR session with 3DGS models
async function startXRSession() {
    const session = await navigator.xr.requestSession('immersive-vr');
    
    // Models automatically adapt to XR rendering pipeline
    const vrScene = document.querySelector('.xr-scene');
    vrScene.innerHTML = `
        <model src="vr_environment.ksplat" class="environment"></model>
        <model src="interactive_objects.ksplat" class="objects"></model>
    `;
    
    session.addEventListener('inputsourceschange', (event) => {
        // 3DGS models automatically receive spatial input
        handleControllerInput(event);
    });
}
```

## Future Enhancements and Roadmap

JSAR's 3DGS implementation continues to evolve with exciting upcoming features:

### Streaming Support
- **Progressive loading** for massive datasets
- **Level-of-detail (LOD)** based on viewing distance
- **Adaptive quality** based on device capabilities

### Advanced Culling
- **Frustum culling** for off-screen splats
- **Occlusion culling** for hidden geometry
- **Distance-based culling** for performance scaling

### Animation Support
- **Temporal splat data** for animated sequences
- **Morphing between states** for smooth transitions
- **Skeletal animation** integration

### Compression Improvements
- **Web-optimized formats** with better compression ratios
- **Streaming-friendly** data organization
- **Hardware-specific optimizations** for different GPU architectures

## Conclusion

JSAR Runtime's integration of 3D Gaussian Splatting into HTML represents a fundamental shift in how we think about 3D content on the web. By treating 3DGS models as first-class HTML elements, JSAR makes photorealistic 3D rendering accessible to every web developer.

The combination of intuitive HTML APIs, aggressive performance optimizations, and seamless WebXR integration creates unprecedented opportunities for immersive web experiences. Whether you're building architectural visualizations, product showcases, or spatial computing applications, JSAR's 3DGS support provides the foundation for next-generation web experiences.

The future of the web is spatial, and with JSAR Runtime, that future is available today through the familiar, accessible technologies that web developers already know and love.

---

*Want to get started with 3DGS in JSAR? Check out our [getting started guide](../manual/getting-started.md) and explore the [3DGS examples](../manual/examples.md) to see these techniques in action.*