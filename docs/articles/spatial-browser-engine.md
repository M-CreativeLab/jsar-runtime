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
| Pillar | Spatial Meaning | JSAR Implementation Signals |
| ------ | --------------- | --------------------------- |
| Spatialized DOM | Elements carry 3D transforms intrinsically | CSS transform functions; layer field per LayoutObject (v0.9.0) |
| Unified Graphics Pipeline | Mixed HTML/SVG/UI + 3D assets in one frame | Layered rendering, offscreen pass + computation graph |
| Standards Compatibility | Reuse Web knowledge | HTML5 subset, CSS3 partial, WebGL1/2, WebXR spaces & input |
| XR & 3D Integration | Stereo, multi-view, spatial inputs | Mono/stereo modes, WebXR input sources (gaze, hand, controller) |
| Performance Batching | Low draw calls despite rich UI | Renderer refactors v0.8.x→v0.9.0; layer calculation |
| Developer Experience | Familiar debugging in XR | CDP Runtime domain, inspector logs & statistics |
| Extensibility | Host engine embedding | Unity loader OK; Unreal planned |

## 5. How JSAR Realizes the Concept
Drawn from changelog (alpha) & README:
- **HTML & DOM**: `<canvas>`, `<img>`, `<audio>`, `HTMLInputElement`, `<template>`, semantic sections, MutationObserver, dataset, unquoted attribute parsing, `document.write`.
- **CSS & Styling**: Transforms (translate/rotate/scale), background-image (url + gradients), background-clip/origin/repeat, blend modes, grid layout, text-transform.
- **Rendering**: Layered rendering, device pixel ratio support, SVG images, environment mapping tests, computation-graph-driven offscreen refinement, renderer refactors with discrete passes.
- **XR Orientation**: Stereo modes (mono/stereo), WebXR spaces & input sources (gaze, hand, controller), spatial transforms persist across passes.
- **Tooling & Debugging**: Chrome DevTools Protocol integration, WebSocket endpoints, MDN-compatible compatibility data structure, statistics/log endpoints.

## 6. Architectural Comparison (Classic vs Spatial)
| Aspect | Classic Browser | JSAR Spatial Engine |
| ------ | --------------- | ------------------- |
| Primary Surface | 2D viewport composited to screen | 3D world / XR reference spaces |
| Transform Stage | Late compositor layer transforms | Native element spatial transform ownership |
| Hit Testing | 2D box model + stacking contexts | Ray/volume intersections w/ depth-aware ordering |
| Draw Submission | Many layers & paints | Intentional batching (≤ ~10 UI draws) |
| Input Sources | Mouse, touch, keyboard | Gaze, hand, controller, (future) eye, anchors |
| Embedding | System window/tab | Library embedded in host 3D engine |
| Pipeline Control | Limited per-pass hooks | Explicit pass lifecycle APIs |

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

## 11. Conclusion
A Spatial Web Browser redefines "page" as a spatial scene graph of standard Web elements. JSAR implements a Spatial Web Browser Engine from first principles to achieve deep 3D integration, performance-focused batching, and XR-native input—all while keeping Web standards familiar. Rewriting (instead of retrofitting) yields architectural clarity and long-term agility.

## 12. Next Steps
For more technical details and hands-on guides:
1. Explore the [User Manual](/manual/introduction) for practical implementation guidance
2. Check the [API Reference](/api/embedder) for integration details  
3. Review the [Changelog](/changelogs/alpha) for recent developments
4. Try the [Unity Loader](https://github.com/M-CreativeLab/jsar-loader-unity) for Unity integration

---
*Prepared referencing README and `docs/changelogs/alpha.md`.*