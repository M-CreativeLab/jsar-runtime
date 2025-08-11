# Understanding the Spatial Web Browser Engine (JSAR)

## 1. What Is a Spatial Web Browser?
A Spatial Web Browser is a user agent that loads, interprets, and presents Web content (HTML, CSS, JS, WebGL/WebGPU, WebXR, media) directly inside a 3D coordinate space instead of flattening everything onto a 2D rectangular viewport. Every DOM element (text nodes, images, canvas, form controls, SVG, etc.) can be:
- Positioned, rotated, and scaled in world / XR reference spaces
- Layered with true depth ordering (not just z-index compositing) for stereoscopic correctness
- Interacted with using spatial input sources (gaze, hands, controllers, future: eye tracking, anchors)
- Composed alongside native 3D assets (GLTF models, environment maps) in one unified frame loop

In short: a Spatial Web Browser lets "regular Web pages" become immersive 3D experiences without abandoning open Web standards.

## 2. Why Not Just Extend an Existing (Classic) Browser?
Traditional engines (Blink, Gecko, WebKit) are extraordinarily capable—but architecturally optimized for 2D document + compositor pipelines. Retrofitting full 3D spatial semantics collides with deep assumptions:
1. **2D Layout Core Assumption**: Flow, inline, block, flex, grid are resolved toward a 2D surface; adding 3D transforms occurs late (compositor), limiting true spatial hit-testing & depth-aware occlusion.
2. **Layer Explosion vs Batching**: Classic engines create layer trees oriented around paint invalidation, often leading to hundreds/thousands of GPU draw submissions. Spatial XR targets demand tight budgets (e.g., ≤ 10–30 draw calls) to hit 90/120Hz.
3. **Input Model**: Mouse/touch event synthesis does not map cleanly to gaze rays, articulated hands, controller pose queries, or multi-reference spaces.
4. **Process & Memory Footprint**: Desktop browsers run heavyweight multi-process sandboxes; embedded XR runtimes (Unity, Unreal, custom engines) need lightweight, embeddable cores.
5. **Render Pipeline Control**: Fine-grained stereo, late latching, foveation, reprojection hooks require explicit staging (before opaque, transparent, after) that monolithic compositor chains obscure.
6. **Security & Surface Area**: Shipping the whole legacy Web surface (plugins, legacy quirks) into an XR app increases attack surface when a narrower, purposely-curated feature subset suffices early on.
7. **Innovation Velocity**: A greenfield engine can evolve DOM → Spatial Layer mapping, GPU instancing strategies, and XR feature integration without legacy regression risk.

Therefore JSAR pursues a purpose-built Spatial Web Browser Engine rather than modifying an existing one.

## 3. Definition: Spatial Web Browser Engine
A Spatial Web Browser Engine is the runnable core (parsing, layout, styling, scripting, rendering, device integration) specialized for spatial presentation. In JSAR this means:
- **Standards Alignment**: HTML5 subset, CSS3 partial, DOM APIs, WebGL1/2, WebXR spaces & inputs
- **Spatialized DOM**: Each element resolves to a layout object enriched with layer + transform metadata
- **Unified Render Passes**: Discrete passes (`onBeforeRendering`, `onOpaquesRenderPass`, `onTransparentsRenderPass`, `onAfterRendering`) integrate seamlessly with host 3D engines
- **Efficient Batching**: Aggressive draw-call minimization for stereoscopic + high-refresh headsets
- **Embeddability**: Loaders (Unity available, Unreal planned) treat the engine as a library
- **Developer Tooling**: Chrome DevTools Protocol (Runtime domain), WebSocket inspector, statistics endpoints

## 4. Core Pillars (Mapped to JSAR Capabilities)

### 4.1 Spatialized DOM
**Concept**: Every HTML element carries intrinsic 3D spatial properties, not just 2D positioning.

**JSAR Implementation**: 
- CSS transform functions work natively in 3D space (`translate3d()`, `rotateX/Y/Z()`, `scale3d()`)
- Layer field metadata per LayoutObject (introduced in v0.9.0) for depth ordering
- Elements can be positioned using spatial units conceptually (meters, not just pixels)

**Example**:
```css
.spatial-panel {
  transform: translate3d(2m, 1.5m, -3m) rotateY(15deg);
  /* Panel positioned 2m right, 1.5m up, 3m forward, rotated 15° */
}
```

### 4.2 Unified Graphics Pipeline
**Concept**: HTML/CSS content and 3D assets render together in a single cohesive frame.

**JSAR Implementation**:
- Layered rendering system with discrete passes
- Offscreen pass + computation graph for mixed content
- WebGL canvas elements seamlessly integrate with HTML DOM elements

**Example**: A virtual museum where `<div>` info panels, `<canvas>` WebGL artifacts, and `<audio>` narration all exist in the same 3D scene with proper depth ordering.

### 4.3 Standards Compatibility
**Concept**: Developers can reuse existing Web knowledge instead of learning proprietary 3D APIs.

**JSAR Implementation**:
- HTML5 subset: `<canvas>`, `<img>`, `<audio>`, form elements, semantic sections
- CSS3 partial: transforms, gradients, grid layout, text styling
- JavaScript APIs: DOM manipulation, WebGL1/2, WebXR Device API
- Modern standards: ECMAScript modules, Web Workers, WebAssembly

**Example**: Existing web developers can create spatial UIs using familiar `<div>`, CSS Grid, and JavaScript without learning Unity or Unreal Engine-specific APIs.

### 4.4 XR & 3D Integration
**Concept**: Native support for immersive viewing modes and spatial interaction paradigms.

#### 4.4.1 Stereo Rendering
**Introduction**: JSAR supports both mono and stereo rendering modes to provide immersive experiences across different display types, from traditional flat screens to advanced XR headsets.

**JSAR Implementation**: The engine automatically detects the display capabilities and switches between mono rendering for standard displays and stereo rendering for VR/AR headsets, ensuring optimal visual fidelity for each platform.

**Example**:
```javascript
// JSAR automatically handles stereo rendering based on device capabilities
const canvas = document.createElement('canvas');
canvas.style.transform = 'translate3d(0, 0, -2m)';
// Content renders in stereo when viewed through VR headset
```

#### 4.4.2 Spatial Audio System
**Introduction**: JSAR provides automatic 3D audio spatialization for all audio elements without requiring complex Web Audio API setup, making spatial audio accessible to all web developers.

**JSAR Implementation**: Every `<audio>` element automatically gains spatial properties based on its 3D transform position, with distance attenuation, directional audio, and environmental acoustics applied automatically.

**Example**:
```html
<audio src="ambient.ogg" autoplay loop 
       style="transform: translate3d(-5m, 2m, 0);"></audio>
<!-- Audio automatically plays from the left side, 2 meters up -->
```

#### 4.4.3 Spatial Images
**Introduction**: JSAR supports stereoscopic images that provide true depth perception in XR environments through the `spatial="stereo"` attribute extension.

**JSAR Implementation**: Images can be marked as stereoscopic, and JSAR handles the proper rendering to each eye for depth effects, supporting both side-by-side and over-under stereo formats.

**Example**:
```html
<img src="stereo-photo.jpg" spatial="stereo" 
     style="transform: translate3d(0, 1.5m, -3m);" />
<!-- Displays with proper depth separation in VR -->
```

#### 4.4.4 WebXR Input Sources
**Introduction**: JSAR integrates seamlessly with WebXR Device API to support various input methods including gaze tracking, hand tracking, and motion controllers.

**JSAR Implementation**: Standard WebXR input events work naturally with spatial DOM elements, allowing developers to use familiar web APIs for spatial interactions.

**Example**:
```javascript
// Standard WebXR input works with spatial elements
document.querySelector('.spatial-button').addEventListener('click', (event) => {
  // Triggered by gaze, hand tracking, or controller input
  console.log('Spatial button clicked:', event.inputSource.handedness);
});
```

### 4.5 Performance Batching
**Concept**: Aggressive optimization to minimize GPU draw calls despite rich spatial UI.

**Why This Is Possible**: JSAR achieves exceptional batching performance because all HTML elements—whether `<div>`, `<p>`, `<img>`, or even text nodes—are fundamentally rendered as textured quads in 3D space. Since every element shares the same underlying geometric primitive (a quad), they can be efficiently batched together with just different positions, sizes, and textures/materials. This unified rendering approach allows hundreds of diverse DOM elements to be drawn in a single GPU call.

**JSAR Implementation**:
- Target ≤10 draw calls per frame (vs hundreds/thousands in classic browsers)
- Renderer refactors from v0.8.x→v0.9.0 with layer calculation optimization
- Dynamic batching of similar spatial elements using shared quad geometry
- Offscreen pass refinement for static vs dynamic content
- Texture atlasing for multiple elements sharing similar styling

**Example**: A complex spatial dashboard with 50+ UI panels batches into just 3-5 GPU draw calls, maintaining 90Hz in VR because all panels are quads with different transforms and textures.

### 4.6 Developer Experience
**Concept**: Familiar debugging tools work in spatial environments.

**JSAR Implementation**:
- Chrome DevTools Protocol (CDP) integration with Runtime domain
- WebSocket inspector endpoints for real-time debugging
- Statistics and log endpoints for performance monitoring
- MDN-compatible API documentation structure

**Example**: Developers can inspect DOM elements, modify CSS, and debug JavaScript in a spatial app using familiar Chrome DevTools.

### 4.7 Extensibility
**Concept**: The browser engine embeds as a library into existing 3D engines and workflows.

**JSAR Implementation**:
- Unity loader available for Unity 3D integration
- Unreal Engine support planned
- Native APIs for custom embedder integration
- Configurable rendering backends (OpenGL, Metal, D3D11)

**Example**: A Unity VR game can embed a JSAR web browser as a virtual computer screen or floating UI panel within the game world.

## 5. How JSAR Realizes the Concept
Drawn from changelog (alpha) & README:
- **HTML & DOM**: `<canvas>`, `<img>`, `<audio>`, `HTMLInputElement`, `<template>`, semantic sections, MutationObserver, dataset, unquoted attribute parsing, `document.write`.
- **CSS & Styling**: Transforms (translate/rotate/scale), background-image (url + gradients), background-clip/origin/repeat, blend modes, grid layout, text-transform.
- **Rendering**: Layered rendering, device pixel ratio support, SVG images, environment mapping tests, computation-graph-driven offscreen refinement, renderer refactors with discrete passes.
- **XR Orientation**: Stereo modes (mono/stereo), WebXR spaces & input sources (gaze, hand, controller), spatial transforms persist across passes.
- **Tooling & Debugging**: Chrome DevTools Protocol integration, WebSocket endpoints, MDN-compatible compatibility data structure, statistics/log endpoints.

## 6. Architectural Comparison (Classic vs Spatial)

### 6.1 Primary Surface Differences
**Classic Browser**: The fundamental rendering target is a 2D rectangular viewport that gets composited to the screen. All content is ultimately flattened onto this 2D surface, even when CSS 3D transforms are applied.

**JSAR Spatial Engine**: The primary surface is a 3D world coordinate system or WebXR reference spaces. Content exists natively in 3D space with true volumetric positioning, not flattened projections.

*Impact*: This architectural difference enables true spatial interfaces where UI elements can exist behind, in front of, or alongside 3D objects with proper occlusion and depth relationships.

### 6.2 Transform Stage Processing
**Classic Browser**: 3D transforms are applied late in the rendering pipeline at the compositor stage. The layout engine still operates in 2D, with 3D effects added as a post-processing step.

**JSAR Spatial Engine**: Elements have native spatial transform ownership from the layout stage. The transform is part of the element's core spatial identity, not an aftereffect.

*Impact*: This enables true spatial hit-testing, depth-aware interactions, and eliminates the disconnect between layout space and visual space.

### 6.3 Hit Testing Methods
**Classic Browser**: Uses 2D box model calculations with stacking context ordering. Ray casting is simulated by projecting 3D transforms back to 2D coordinates.

**JSAR Spatial Engine**: Implements true ray/volume intersection testing with depth-aware ordering. Spatial elements can be tested for intersection in 3D space using their actual geometric bounds.

*Impact*: Enables natural spatial interaction paradigms like gaze-based selection, hand tracking, and controller pointing with accurate depth perception.

### 6.4 Draw Submission Strategy
**Classic Browser**: Generates hundreds to thousands of GPU draw calls per frame due to layer-based rendering optimization for 2D painting and invalidation. Each CSS layer, text run, and visual effect can create separate draw calls.

**JSAR Spatial Engine**: Employs intentional batching strategies targeting ≤10 draw calls per frame for spatial UI elements. Uses aggressive batching, instancing, and spatial culling to meet XR performance requirements (90-120Hz).

*Impact*: This dramatic reduction in draw calls is essential for XR headsets where GPU budget is limited and frame rate drops cause motion sickness.

### 6.5 Input Source Architecture
**Classic Browser**: Designed around mouse, touch, and keyboard input. XR inputs are emulated by synthesizing mouse events from controller positions.

**JSAR Spatial Engine**: Native support for spatial input sources including gaze rays, articulated hand tracking, 6-DOF controllers, and future technologies like eye tracking and spatial anchors.

*Impact*: Enables natural multimodal interaction in spatial environments without the limitations of mouse event emulation.

### 6.6 Embedding Model
**Classic Browser**: Operates as a system-level application creating windows or tabs. Designed to be the primary application rather than a component.

**JSAR Spatial Engine**: Functions as a library that embeds within host 3D engines (Unity, Unreal, custom). The spatial browser becomes a component within larger 3D applications.

*Impact*: Allows existing 3D applications and games to incorporate web content as spatial UI elements or information panels.

### 6.7 Pipeline Control Granularity
**Classic Browser**: Provides limited hooks into the rendering pipeline. Developers can influence layout and styling but have minimal control over the actual rendering passes.

**JSAR Spatial Engine**: Exposes explicit pass lifecycle APIs (`onBeforeRendering`, `onOpaquesRenderPass`, `onTransparentsRenderPass`, `onAfterRendering`) allowing fine-grained integration with host rendering systems.

*Impact*: Enables advanced rendering techniques like custom depth sorting, multi-pass effects, and integration with existing 3D engine rendering pipelines.

## 7. Example Scenarios
### 7.1 Virtual Museum Exhibit
Panels (`<div>` with CSS Grid); 3D artifact models (WebGL); `<audio>` narration; dynamic `<canvas>` analytics; depth ordering prevents visual collision.

### 7.2 AR Field Maintenance HUD
Procedural step panels anchored to machine parts via CSS transforms; MutationObserver updates sensor-fed DOM nodes; stereoscopic clarity reduces technician cognitive load.

### 7.3 Multi-Window Spatial Dashboard (VR)
Multiple document contexts (future `<iframe>`) arranged on a curved console; background blend modes indicate focus depth; hand/controller input repositions panels.

## 8. Minimal Spatial UI Code Example
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

## 9. Performance Considerations
- **Layer Field Calculation**: Guides batching & z/depth ordering (v0.9.0)
- **Offscreen Pass Refinement**: Only dynamic surfaces re-render; static panels reused
- **DPR Support**: Crisp text in XR while avoiding oversampling
- **Future**: Foveated layout invalidation; GPU-driven instancing for repeated subtrees; spatial occlusion culling; WebGPU backend for parallelism

## 10. Roadmap (Inferred)
- **Expanded HTML**: video, richer form controls, web components
- **Advanced XR**: Hit Test, Anchors, Light Estimation, Eye Tracking integration
- **Audio**: Full Web Audio graph + spatial mixing refinements
- **Rendering Backends**: Metal, D3D11 parity; eventual WebGPU path
- **Security & Sandboxing**: Multi-document via `<iframe>` with isolation boundaries
- **High-Level Spatial UI Primitives**: Radial menus, volumetric scrollers, anchored HUD frameworks

## 11. JSAR-Specific Spatial Features

### 11.1 Spatial Audio System
JSAR provides automatic **3D audio spatialization** for all audio content without requiring complex Web Audio API setup.

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

### 11.2 Spatial Images (Stereoscopic Support)
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

## 12. Conclusion
A Spatial Web Browser redefines "page" as a spatial scene graph of standard Web elements. JSAR implements a Spatial Web Browser Engine from first principles to achieve deep 3D integration, performance-focused batching, and XR-native input—all while keeping Web standards familiar. Rewriting (instead of retrofitting) yields architectural clarity and long-term agility.

---
*Prepared referencing README and `docs/changelogs/alpha.md`.*