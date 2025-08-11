# Understanding the Spatial Web Browser Engine (JSAR)

## 1. What Is a Spatial Web Browser?

A Spatial Web Browser is a user agent that loads, interprets, and presents Web content ([HTML][HTML], [CSS][CSS], [JS][JavaScript], [WebGL][WebGL]/[WebGPU][WebGPU], [WebXR][WebXR], media) directly inside a 3D coordinate space instead of flattening everything onto a 2D rectangular viewport. Every [DOM][DOM] element (text nodes, images, canvas, form controls, [SVG][SVG], etc.) can be:
- Positioned, rotated, and scaled in world / XR reference spaces
- Layered with true depth ordering (not just z-index compositing) for stereoscopic correctness
- Interacted with using spatial input sources (gaze, hands, controllers, future: eye tracking, anchors)
- Composed alongside native 3D assets ([GLTF][GLTF] models, environment maps) in one unified frame loop

In short: a Spatial Web Browser lets "regular Web pages" become immersive 3D experiences without abandoning open Web standards.

## 2. Why Not Just Extend an Existing (Classic) Browser?

Traditional engines ([Blink][Blink], [Gecko][Gecko], [WebKit][WebKit]) are extraordinarily capable—but architecturally optimized for 2D document + compositor pipelines. Retrofitting full 3D spatial semantics collides with deep assumptions:
1. **XR Spatial Integration Requirements**: In XR systems, we need to seamlessly integrate Web content with other [Unity][Unity]/[Unreal][Unreal] 3D engine content in spatial environments, which traditional browsers' 2D architecture cannot satisfy.
2. **Full 3D Element Design**: Inspired by visionOS, in Spatial Web, we need all elements to have true 3D spatial properties rather than being confined to a flat plane, requiring a complete architectural redesign from the ground up.

Therefore JSAR pursues a purpose-built Spatial Web Browser Engine rather than modifying an existing one.

This approach contrasts with the evolutionary path taken by visionOS Safari, which gradually introduces spatial features like HTMLModelElement for 3D model display and Spatial Photo support for stereoscopic images. While visionOS represents a progressive enhancement of traditional browser engines toward spatial capabilities, JSAR chose a fundamentally different direction—designing for spatial (3D) environments from the ground up rather than retrofitting existing 2D architectures.

Both approaches ultimately converge toward the same goal: enabling web developers to create true 3D applications using familiar web technologies without learning new frameworks or specialized knowledge. Whether through gradual evolution or ground-up redesign, the future of spatial web development will remain accessible to existing web developers while unlocking the full potential of three-dimensional user interfaces.

## 3. Definition: Spatial Web Browser Engine

A Spatial Web Browser Engine is the runnable core (parsing, layout, styling, scripting, rendering, device integration) specialized for spatial presentation. In JSAR this means:
- **Standards Alignment**: [HTML5][HTML5] subset, [CSS3][CSS3] partial, [DOM APIs][DOM], [WebGL1/2][WebGL], [WebXR][WebXR] spaces & inputs
- **Spatialized DOM**: Each element resolves to a layout object enriched with layer + transform metadata
- **Unified Render Passes**: Discrete passes (`onBeforeRendering`, `onOpaquesRenderPass`, `onTransparentsRenderPass`, `onAfterRendering`) integrate seamlessly with host 3D engines
- **Efficient Batching**: Aggressive draw-call minimization for stereoscopic + high-refresh headsets
- **Embeddability**: Loaders ([Unity][Unity] available, [Unreal][Unreal] planned) treat the engine as a library
- **Developer Tooling**: [Chrome DevTools Protocol][CDP] (Runtime domain), [WebSocket][WebSocket] inspector

## 4. Core Capabilities

### 4.1 Spatialized DOM

Every HTML element carries intrinsic 3D spatial properties, not just 2D positioning.

- [CSS transform][CSS-Transform] functions work natively in 3D space (`translate3d()`, `rotateX/Y/Z()`, `scale3d()`)
- Layer field metadata per `LayoutObject` for depth ordering
- Elements can be positioned using spatial units conceptually (meters, not just pixels)

**Example**:
```css
.spatial-panel {
  transform: translate3d(2px, 1.5px, -3px) rotateY(15deg);
  /* Panel positioned 2px right, 1.5px up, 3px forward, rotated 15° */
}
```

### 4.2 Unified Graphics Pipeline

HTML elements and 3D content can be seamlessly integrated in JSAR, enabling powerful combinations like HTML + [Three.js][Three.js] for mixed reality applications. HTML serves as the foundation for implementing Spatialized GUI components, while [Three.js][Three.js] handles 3D model rendering and complex 3D applications within the XR space.

**JSAR's WebGL/WebXR Foundation**:
JSAR achieves this integration through comprehensive support for [WebGL][WebGL] and [WebXR][WebXR], allowing popular frameworks like [Three.js][Three.js], [Babylon.js][Babylon.js], or custom web rendering engines to run natively on the platform.

**Key Difference in Initialization**:
Unlike traditional browsers where you request a canvas element and draw scenes onto it, XR spatial environments don't require a canvas. Instead, JSAR provides direct access to `navigator.gl` - a [WebGL2][WebGL] context that allows you to draw objects directly into the spatial environment.

**Example**: A virtual museum implementation demonstrating HTML + Three.js integration:

```html
<html>
<head>
  <meta charset="utf-8" />
  <title>Virtual Museum - JSAR Spatial Browser</title>
  <script type="importmap">
    {
      "imports": {
        "three": "https://ar.rokidcdn.com/web-assets/yodaos-jsar/dist/three/build/three.module.js"
      }
    }
  </script>
  <style>
    .info-panel {
      position: absolute;
      background: rgba(0, 0, 0, 0.8);
      color: white;
      padding: 20px;
      border-radius: 10px;
      width: 300px;
      transform: translateZ(100px);
    }
    .artifact-info {
      left: 50px;
      top: 100px;
    }
    .controls {
      right: 50px;
      top: 100px;
    }
  </style>
</head>
<body>
  <!-- HTML UI Panels for Interactive Information -->
  <div class="info-panel artifact-info">
    <h3>Ancient Vase</h3>
    <p>Ming Dynasty ceramic vase from 1368-1644 CE</p>
    <button>Highlight</button>
  </div>
  
  <div class="info-panel controls">
    <h3>Museum Controls</h3>
    <button>Next Exhibit</button>
  </div>

  <script type="module">
    import * as THREE from 'three';
    
    let scene, camera, renderer;
    let artifactMesh;
    
    async function init() {
      // Initialize Three.js with navigator.gl (no canvas needed)
      const gl = navigator.gl;
      
      try {
        const session = await navigator.xr.requestSession('immersive-ar', {});
        const baseLayer = new XRWebGLLayer(session, gl);
        session.updateRenderState({ baseLayer });
        
        renderer = new THREE.WebGLRenderer({
          canvas: { addEventListener() {} },
          context: gl,
        });
        renderer.xr.enabled = true;
        renderer.xr.setSession(session);
        
        scene = new THREE.Scene();
        camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
        
        // Create 3D artifact (vase)
        const geometry = new THREE.CylinderGeometry(0.5, 0.3, 1, 32);
        const material = new THREE.MeshPhongMaterial({ color: 0x8B4513 });
        artifactMesh = new THREE.Mesh(geometry, material);
        artifactMesh.position.set(0, 0, -2);
        scene.add(artifactMesh);
        
        // Add lighting
        const ambientLight = new THREE.AmbientLight(0x404040, 0.6);
        const directionalLight = new THREE.DirectionalLight(0xffffff, 0.8);
        directionalLight.position.set(1, 1, 1);
        scene.add(ambientLight);
        scene.add(directionalLight);
        
        // Start render loop
        renderer.setAnimationLoop(animate);
      } catch (err) {
        console.error('Failed to initialize XR:', err);
      }
    }

    function animate() {
      artifactMesh.rotation.y += 0.01;
      renderer.render(scene, camera);
    }

    init();
  </script>
</body>
</html>
```

This example demonstrates how HTML `<div>` panels provide interactive UI controls and [Three.js][Three.js] renders 3D artifacts using `navigator.gl` instead of canvas - both coexisting in the same 3D scene with proper depth ordering and spatial relationships.

### 4.3 Standards Compatibility

Developers can reuse existing Web knowledge instead of learning proprietary 3D APIs.

- [HTML5][HTML5] subset: `<canvas>`, `<img>`, `<audio>`, form elements, semantic sections
- [CSS3][CSS3] partial: transforms, gradients, [grid layout][CSS-Grid], text styling
- JavaScript APIs: [DOM manipulation][DOM], [WebGL1/2][WebGL], [WebXR Device API][WebXR]
- Modern standards: [ECMAScript modules][ES-Modules], [Web Workers][Web-Workers], [WebAssembly][WebAssembly]

**Example**: Existing web developers can create spatial UIs using familiar `<div>`, [CSS Grid][CSS-Grid], and JavaScript without learning [Unity][Unity] or [Unreal Engine][Unreal]-specific APIs.

### 4.4 XR & 3D Integration

Native support for immersive viewing modes and spatial interaction paradigms.

#### 4.4.1 Stereo Rendering

JSAR supports both mono and stereo rendering modes to provide immersive experiences across different display types and platforms:

**Native Binocular Devices**: On devices with natural dual-eye displays (VR/AR headsets), JSAR enforces stereo mode for dual-eye rendering. All Web Content is automatically rendered from both left and right eye perspectives, creating true stereoscopic 3D experiences.

**Desktop Platforms**: Users can choose between mono and stereo rendering modes for Web content. This flexibility allows developers to test stereo experiences on traditional displays or provide optional 3D viewing modes.

#### 4.4.2 Spatial Audio System

JSAR provides automatic 3D audio spatialization for all audio elements without requiring complex [Web Audio API][Web-Audio] setup, making spatial audio accessible to all web developers.

Every `<audio>` element automatically gains spatial properties based on its 3D transform position, with distance attenuation, directional audio, and environmental acoustics applied automatically.

**Example**:
```html
<audio src="ambient.ogg" autoplay loop></audio>
<!-- Audio automatically plays from the left side, 2 meters up -->
```

When users' positions change relative to the page in space (whether due to user movement or HTML panel movement), they can perceive the distance and position changes of sound sources through audio cues. This dynamic spatial audio positioning provides users with natural auditory feedback, enhancing the sense of immersion and directionality in spatial environments.

#### 4.4.3 Spatial Images

JSAR supports stereoscopic images that provide true depth perception in XR environments through the `spatial="stereo"` attribute extension. JSAR handles the proper rendering to each eye for depth effects, supporting side-by-side stereo format.

**Example**:
```html
<img src="stereo-photo.jpg" spatial="stereo" />
<!-- Displays with proper depth separation in VR -->
```

#### 4.4.4 WebXR Input Sources

JSAR internally leverages [WebXR Input Sources API][WebXR] to bridge traditional DOM events with XR device interactions, supporting various input methods including gaze tracking, hand tracking, and motion controllers.

By mapping XR device interactions to native DOM events (such as `mouseup`, `mousemove`, `click`), JSAR allows web developers to use familiar DOM event listeners to respond to gestures, controller rays, touchpads, and other XR inputs without needing to write additional [WebXR Input Source][WebXR] code.

**Example**:
```javascript
// Standard WebXR input works with spatial elements
document.querySelector('.spatial-button').addEventListener('click', (event) => {
  // Triggered by gaze, hand tracking, or controller input
  console.log('Spatial button clicked:', event);
});
```

### 4.5 Performance Batching

**Design Philosophy: Inherently 3D HTML Elements**

JSAR makes every HTML element inherently 3D without requiring explicit user declarations, driven by three core principles:

1. **Content-Focused Development**: We enable users and AI assistants like Copilot to write HTML based on what they want to express, rather than being constrained by performance considerations. Developers can focus on semantic content without worrying about spatial rendering overhead.

2. **Complete Platform Backward Compatibility**: JSAR provides a seamless cross-platform development experience without requiring additional XR-specific declarations. Existing web content works naturally in spatial environments, ensuring developers aren't limited to XR-only applications.

3. **Spatial Reality Assumption**: In spatial contexts, any 2D element should be fundamentally 3D by nature. Based on this assumption, JSAR implements aggressive optimization using instanced draw calls to minimize GPU overhead while maintaining rich spatial GUI capabilities.

JSAR achieves exceptional batching performance because all HTML elements—whether `<div>`, `<p>`, `<img>`, or even text nodes—are fundamentally rendered as textured quads in 3D space. Since every element shares the same underlying geometric primitive (a quad), they can be efficiently batched together with just different positions, sizes, and textures/materials. This unified rendering approach allows thousands of diverse DOM elements to be drawn in a single GPU call.

### 4.6 Developer Experience

Familiar debugging tools work in spatial environments.

- Basic [Chrome DevTools Protocol][CDP] (CDP) WebSocket service and communication protocol
- Foundation for real-time debugging infrastructure
- General Domain support coming soon once the system stabilizes

### 4.7 Extensibility

The browser engine embeds as a library into existing 3D engines and workflows.

- [Unity][Unity] loader available for Unity 3D integration
- [Unreal Engine][Unreal] support planned
- Native APIs for custom embedder integration

## 5. Architectural Comparison (Classic vs Spatial)

### 5.1 Primary Surface Differences

**Classic Browser**: The fundamental rendering target is a 2D rectangular viewport that gets composited to the screen. All content is ultimately flattened onto this 2D surface, even when [CSS 3D transforms][CSS-Transform] are applied.

**JSAR Spatial Engine**: The primary surface is a 3D world coordinate system or [WebXR reference spaces][WebXR-Geometry]. Content exists natively in 3D space with true volumetric positioning, not flattened projections.

*Impact*: This architectural difference enables true spatial interfaces where UI elements can exist behind, in front of, or alongside 3D objects with proper occlusion and depth relationships.

### 5.2 Transform Stage Processing

**Classic Browser**: 3D transforms are applied late in the rendering pipeline at the compositor stage. The layout engine still operates in 2D, with 3D effects added as a post-processing step.

**JSAR Spatial Engine**: Elements have native spatial transform ownership from the layout stage. The transform is part of the element's core spatial identity, not an aftereffect.

*Impact*: This enables true spatial hit-testing, depth-aware interactions, and eliminates the disconnect between layout space and visual space.

### 5.3 Hit Testing Methods
**Classic Browser**: Uses 2D box model calculations with stacking context ordering. Ray casting is simulated by projecting 3D transforms back to 2D coordinates.

**JSAR Spatial Engine**: Implements true ray/volume intersection testing with depth-aware ordering. Spatial elements can be tested for intersection in 3D space using their actual geometric bounds.

*Impact*: Enables natural spatial interaction paradigms like gaze-based selection, hand tracking, and controller pointing with accurate depth perception.

### 5.4 Draw Submission Strategy
**Classic Browser**: Generates hundreds to thousands of GPU draw calls per frame due to layer-based rendering optimization for 2D painting and invalidation. Each CSS layer, text run, and visual effect can create separate draw calls.

**JSAR Spatial Engine**: Employs intentional batching strategies targeting ≤10 draw calls per frame for spatial UI elements. Uses aggressive batching, instancing, and spatial culling to meet XR performance requirements (90-120Hz).

*Impact*: This dramatic reduction in draw calls is essential for XR headsets where GPU budget is limited and frame rate drops cause motion sickness.

### 5.5 Input Source Architecture
**Classic Browser**: Designed around mouse, touch, and keyboard input. XR inputs are emulated by synthesizing mouse events from controller positions.

**JSAR Spatial Engine**: Native support for spatial input sources including gaze rays, articulated hand tracking, 6-DOF controllers, and future technologies like eye tracking and spatial anchors.

*Impact*: Enables natural multimodal interaction in spatial environments without the limitations of mouse event emulation.

### 5.6 Embedding Model
**Classic Browser**: Operates as a system-level application creating windows or tabs. Designed to be the primary application rather than a component.

**JSAR Spatial Engine**: Functions as a library that embeds within host 3D engines ([Unity][Unity], [Unreal][Unreal], custom). The spatial browser becomes a component within larger 3D applications.

*Impact*: Allows existing 3D applications and games to incorporate web content as spatial UI elements or information panels.

### 5.7 Pipeline Control Granularity
**Classic Browser**: Provides limited hooks into the rendering pipeline. Developers can influence layout and styling but have minimal control over the actual rendering passes.

**JSAR Spatial Engine**: Exposes explicit pass lifecycle APIs (`onBeforeRendering`, `onOpaquesRenderPass`, `onTransparentsRenderPass`, `onAfterRendering`) allowing fine-grained integration with host rendering systems.

*Impact*: Enables advanced rendering techniques like custom depth sorting, multi-pass effects, and integration with existing 3D engine rendering pipelines.

## 6. Example Scenarios
### 6.1 Virtual Museum Exhibit
Panels (`<div>` with [CSS Grid][CSS-Grid]); 3D artifact models ([WebGL][WebGL]); `<audio>` narration; dynamic `<canvas>` analytics; depth ordering prevents visual collision.

### 6.2 AR Field Maintenance HUD
Procedural step panels anchored to machine parts via [CSS transforms][CSS-Transform]; [MutationObserver][MutationObserver] updates sensor-fed DOM nodes; stereoscopic clarity reduces technician cognitive load.

### 6.3 Multi-Window Spatial Dashboard (VR)
Multiple document contexts (future `<iframe>`) arranged on a curved console; background blend modes indicate focus depth; hand/controller input repositions panels.

## 7. Minimal Spatial UI Code Example
```html
<!DOCTYPE html>
<html>
  <head>
    <title>Spatial Control Panel</title>
    <style>
      body { font-family: sans-serif; }
      .panel {
        width: 320px; padding: 16px; border-radius: 12px;
        background: linear-gradient(135deg,#1e293b,#334155);
        color: #fff; backdrop-filter: blur(6px);
        /* Spatial transform (conceptual example) */
        transform: translate3d(0, 0, -1.2m) rotateY(15deg) scale(1.0);
      }
      .metric { font-size: 2rem; margin: 12px 0; }
    </style>
  </head>
  <body>
    <div class="panel" id="status">
      <h2>System Metrics</h2>
      <div class="metric" id="temp">Temp: -</div>
      <canvas id="trend" width="320" height="120"></canvas>
    </div>
    <script type="module">
      const tempEl = document.getElementById('temp');
      setInterval(()=>{
        const v = (20 + Math.random()*5).toFixed(2);
        tempEl.textContent = `Temp: ${v}°C`;
      }, 1000);
    </script>
  </body>
</html>
```
*(Units like meters could map to engine space; actual CSS length resolution may differ.)*

## 8. Performance Considerations
- **Layer Field Calculation**: Guides batching & z/depth ordering (v0.9.0)
- **Offscreen Pass Refinement**: Only dynamic surfaces re-render; static panels reused
- **DPR Support**: Crisp text in XR while avoiding oversampling
- **Future**: Foveated layout invalidation; GPU-driven instancing for repeated subtrees; spatial occlusion culling; [WebGPU][WebGPU] backend for parallelism

## 9. Roadmap (Inferred)
- **Expanded HTML**: video, richer form controls, [web components][Web-Components]
- **Advanced XR**: Hit Test, Anchors, Light Estimation, Eye Tracking integration
- **Audio**: Full [Web Audio][Web-Audio] graph + spatial mixing refinements
- **Rendering Backends**: [Metal][Metal], [D3D11][D3D11] parity; eventual [WebGPU][WebGPU] path
- **Security & Sandboxing**: Multi-document via `<iframe>` with isolation boundaries
- **High-Level Spatial UI Primitives**: Radial menus, volumetric scrollers, anchored HUD frameworks

## 10. JSAR-Specific Spatial Features

### 10.1 Spatial Audio System
JSAR provides automatic **3D audio spatialization** for all audio content without requiring complex [Web Audio API][Web-Audio] setup.

**Key Features**:
- **Automatic Positioning**: Every `<audio>` element is automatically positioned in 3D space based on its containing element's spatial transform
- **No Manual Setup**: Unlike traditional browsers that require `AudioListener` and `PannerNode` configuration, JSAR handles spatialization automatically
- **Natural Audio Falloff**: Audio volume and stereo positioning change naturally as users move through the spatial environment

**Example**:
```html
<!-- Audio automatically positioned at the panel's 3D location -->
<div class="info-panel" style="transform: translate3d(2m, 1m, -3m);">
  <h2>Historical Artifact</h2>
  <audio src="narration.mp3" autoplay loop></audio>
</div>
```

As users move around the spatial scene, they can naturally locate the audio source by ear, creating intuitive spatial navigation and immersive audio experiences.

### 10.2 Spatial Images (Stereoscopic Support)
JSAR natively supports **spatial images** that display different content to each eye for true 3D stereoscopic effects.

**Key Features**:
- **Stereo Attribute**: Simple `spatial="stereo"` attribute on standard `<img>` elements
- **Side-by-Side Format**: Uses standard side-by-side stereo image format (left half for left eye, right half for right eye)
- **Graceful Fallback**: Displays as regular images in non-stereo environments
- **Standard Integration**: Works with all normal image features (CSS styling, JavaScript manipulation)

**Example**:
```html
<!-- Regular image -->
<img src="regular-photo.jpg" style="width: 300px; height: 200px;" />

<!-- Spatial stereo image -->
<img src="stereo-landscape.jpg" 
     spatial="stereo" 
     style="width: 300px; height: 150px;" 
     alt="3D landscape view" />
```

**JavaScript API**:
```javascript
// Create spatial images dynamically
const spatialImg = document.createElement('img');
spatialImg.src = 'stereo-content.jpg';
spatialImg.setAttribute('spatial', 'stereo');
document.body.appendChild(spatialImg);

// Check if image is spatial
if (img.getAttribute('spatial') === 'stereo') {
  console.log('This image has 3D depth');
}
```

These spatial images enable developers to create immersive photo galleries, 3D documentation, and stereoscopic user interfaces using familiar HTML image elements.

## 11. Conclusion
A Spatial Web Browser redefines "page" as a spatial scene graph of standard Web elements. JSAR implements a Spatial Web Browser Engine from first principles to achieve deep 3D integration, performance-focused batching, and XR-native input—all while keeping Web standards familiar. Rewriting (instead of retrofitting) yields architectural clarity and long-term agility.

---
*Prepared referencing README and `docs/changelogs/alpha.md`.*

[HTML]: https://developer.mozilla.org/en-US/docs/Web/HTML
[CSS]: https://developer.mozilla.org/en-US/docs/Web/CSS
[JavaScript]: https://developer.mozilla.org/en-US/docs/Web/JavaScript
[WebGL]: https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API
[WebGPU]: https://developer.mozilla.org/en-US/docs/Web/API/WebGPU_API
[WebXR]: https://developer.mozilla.org/en-US/docs/Web/API/WebXR_Device_API
[DOM]: https://developer.mozilla.org/en-US/docs/Web/API/Document_Object_Model
[SVG]: https://developer.mozilla.org/en-US/docs/Web/SVG
[Three.js]: https://threejs.org/
[GLTF]: https://www.khronos.org/gltf/
[Babylon.js]: https://www.babylonjs.com/
[Blink]: https://www.chromium.org/blink/
[Gecko]: https://developer.mozilla.org/en-US/docs/Mozilla/Gecko
[WebKit]: https://webkit.org/
[Unity]: https://unity.com/
[Unreal]: https://www.unrealengine.com/
[HTML5]: https://developer.mozilla.org/en-US/docs/Web/Guide/HTML/HTML5
[CSS3]: https://developer.mozilla.org/en-US/docs/Web/CSS/CSS3
[CDP]: https://chromedevtools.github.io/devtools-protocol/
[WebSocket]: https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API
[CSS-Transform]: https://developer.mozilla.org/en-US/docs/Web/CSS/transform
[CSS-Grid]: https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_Grid_Layout
[ES-Modules]: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Modules
[Web-Workers]: https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API
[WebAssembly]: https://developer.mozilla.org/en-US/docs/WebAssembly
[Web-Audio]: https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API
[Chrome-DevTools]: https://developer.chrome.com/docs/devtools/
[MDN]: https://developer.mozilla.org/
[OpenGL]: https://www.opengl.org/
[Metal]: https://developer.apple.com/metal/
[D3D11]: https://docs.microsoft.com/en-us/windows/win32/direct3d11/atoc-dx-graphics-direct3d-11
[MutationObserver]: https://developer.mozilla.org/en-US/docs/Web/API/MutationObserver
[WebXR-Geometry]: https://developer.mozilla.org/en-US/docs/Web/API/WebXR_Device_API/Geometry
[Web-Components]: https://developer.mozilla.org/en-US/docs/Web/Web_Components