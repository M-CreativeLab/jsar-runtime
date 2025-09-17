# JSAR Alpha Changelog

The alpha version of JSAR is the first version of the project.

## v0.10.1

### New Features & Improvements

#### DOM & Web APIs
- **Element.onclick Support**: Added support for Element.onclick attribute for inline event handlers (#309)
- **Text Alignment Tests**: Added comprehensive text-align test cases to fixtures/html (#313)

#### CSS & Styling
- **Flexbox Properties**: Support parsing flex-grow and flex-shrink at computed style
- **CSS Selectors**: Implement CSS nth-child() and nth-of-type() selector support (#285)
- **CSS Variables**: Support CSS variables (#263)
- **Universal Selector**: Support universal selector (*) in CSSOM selector parser and matching (#261)
- **Pseudo-class Selectors**: 
  - Implement CSS `:root` selector matching (#259)
  - Implement CSS :where() functional pseudo-class selector support (#258)
  - Replace Rust CSS selectors with C++ implementation and add pseudo-class support (#241)
- **Attribute Selectors**: Implement CSS attribute selector support in parser and matcher (#271)
- **Background Properties**: Support using background-position, background-size and background-repeat (#274)
- **Background Repeat**: Background-repeat should draw from clipped area (#280)
- **Absolute Positioning**: Fix absolute positioning to reference nearest positioned ancestor (#276)
- **Align Items**: Correct align-items implementation (#282)

#### Canvas & WebGL
- **Canvas 2D**: Implement strokeRect() method in Canvas 2D rendering context (#294)
- **WebGL**: Implement WebGL validateProgram method (#253)

#### Browser & Rendering
- **Parallel Rendering**: Use std::async to make each web content rendering run in parallel (#310)
- **Texture Bindings**: Unbind texture bindings correctly (#221)
- **3D Content**: Add 3D world-space content bar and input box (#269)
- **Frame Rate Control**: Add frame rate control to vsync and manual modes (#265)
- **Console Logging**: Implement Console binding to support logging on CDP (#267)

#### Testing & Quality Assurance
- **Border Tests**: Add comprehensive border regression test page to fixtures/html (#302)
- **Text Rendering Tests**: Add comprehensive text rendering test page for fixtures (#301)
- **Flexbox Tests**: Add flex-grow and flex-shrink test case in fixtures/html (#300)
- **Canvas Tests**: Add comprehensive Canvas API test pages to fixtures/html (#293)
- **CSS Overflow Tests**: Add comprehensive CSS overflow test file to fixtures/html (#290)
- **Layout Tests**: Add flexbox and grid layout test pages (#282)

### Bug Fixes & Performance

#### Rendering & Graphics
- **Render Queue**: Correct RenderQueue sorting for positioned elements (#297)
- **Depth Writing**: 
  - Correct depth write only pass for web content
  - Write web content depth should be written with stencil testing (#272)
  - Correct the bar component depth writes
- **Build Fixes**: Fix build errors when enabling TR_RENDERER_ENABLE_VERBOSE
- **GLSL Processing**: Upgrade glsl-lang to fix the glsl preprocessing issues (#295)

#### DOM & Browser
- **Scroll Throttling**: Implement scroll throttling in document and fix document dirty state (#289)
- **Font Manager**: Maintain only one instance for font manager (#286)
- **Rendering Parameters**: Tweak the web content rendering parameters

#### Documentation & Infrastructure
- **Commit Guidelines**: Update commit message guidelines (#311)
- **Three.js Examples**: Update threejs example sample

### Detailed Commit History

| Commit | Description | PR |
|--------|-------------|-----|
| af078628 | feat(tests): add comprehensive text-align test cases to fixtures/html | #313 |
| bdf2771a | fix(tests): update threejs example sample | - |
| 83a628aa | feat(dom): support Element.onclick attribute for inline event handlers | #309 |
| f1b92b1c | docs(docs-infra): update commit message guidelines | #311 |
| 4e013a3b | fix(builtin_scene): use std::async to make each web content rendering run in parallel | #310 |
| f918ddfc | fix(builtin_scene): unbind texture bindings correctly | #221 |
| 86de83ef | fix(renderer): fix build errors when enabling TR_RENDERER_ENABLE_VERBOSE | - |
| 562a4838 | add comprehensive border regression test page to fixtures/html | #302 |
| 6b930312 | add comprehensive text rendering test page for fixtures | #301 |
| 366a0589 | fix(builtin_scene): tweak the web content rendering parameters | - |
| 60f3d278 | add flex-grow and flex-shrink test case in fixtures/html | #300 |
| 5145c149 | feat(css): support parsing flex-grow and flex-shrink at computed style | - |
| 070211e1 | fix(dom): correct RenderQueue sorting for positioned elements | #297 |
| 397aac76 | fix(css): fix absolute positioning to reference nearest positioned ancestor | #276 |
| c9837bb4 | fix(dep): upgrade glsl-lang to fix the glsl preprocessing issues | #295 |
| b5a086f4 | feat(canvas): implement strokeRect() method in Canvas 2D rendering context | #294 |
| da43422b | feat(test): add comprehensive Canvas API test pages to fixtures/html | #293 |
| 91564530 | feat(test): add comprehensive CSS overflow test file to fixtures/html | #290 |
| 1668735d | fix(dom): implement scroll throttling in document and fix document dirty state | #289 |
| 0b3a535e | feat(webgl): implement WebGL validateProgram method | #253 |
| 8b61cbe1 | fix(browser): maintain only one instance for font manager | #286 |
| 2f5296a2 | feat(cssom): implement CSS nth-child() and nth-of-type() selector support | #285 |
| 4b09aab0 | fix(cssom): correct align-items impl and add flexbox and grid layout test pages | #282 |
| 9b9cba37 | fix(cssom): background-repeat should draw from clipped area | #280 |
| 8e3b8ae5 | feat(cssom): support using background-position, background-size and background-repeat | #274 |
| 404457ac | fix(browser): correct the bar component depth writes | - |
| 8c46b022 | feat(bindings): Implement Console binding to support logging on cdp | #267 |
| 46a910f9 | fix(builtin_scene): correct depth write only pass for web content | - |
| d660b572 | fix(builtin_scene): write web content depth should be written with stencil testing | #272 |
| 69344ad9 | feat(cssom): implement CSS attribute selector support in parser and matcher | #271 |
| b8bab310 | feat(browser): add 3D world-space content bar and input box | #269 |
| 88e86cff | feat(browser): add frame rate control to vsync and manual modes | #265 |
| 204c7618 | feat(cssom): support CSS variables | #263 |
| 99b7458d | feat(cssom): support universal selector (*) in CSSOM selector parser and matching | #261 |
| bb2894ba | feat(cssom): implement CSS `:root` selector matching | #259 |
| 0529c45e | feat(cssom): implement CSS :where() functional pseudo-class selector support | #258 |
| 19dafc6b | feat(cssom): replace Rust CSS selectors with C++ implementation and add pseudo-class support | #241 |

### Regression Testing Cases

| File | Description |
|------|-------------|
| `fixtures/html/element-onclick-test.html` | Element.onclick attribute for inline event handlers (#309) |
| `fixtures/html/text-rendering-test.html` | Comprehensive text rendering test page (#301) |
| `fixtures/html/border-regression-test.html` | Comprehensive border regression test page (#302) |
| `fixtures/html/layout-flexbox-grow-shrink.html` | Flex-grow and flex-shrink test case (#300) |
| `fixtures/html/canvas-api-test-suite.html` | Comprehensive Canvas API test pages (#293) |
| `fixtures/html/canvas-api-basic-drawing.html` | Canvas drawing operations including strokeRect() (#294) |
| `fixtures/html/canvas-api-paths.html` | Canvas path operations |
| `fixtures/html/canvas-api-state.html` | Canvas state management |
| `fixtures/html/canvas-api-styles.html` | Canvas styling |
| `fixtures/html/canvas-api-text.html` | Canvas text rendering |
| `fixtures/html/canvas-api-transforms.html` | Canvas transformations |
| `fixtures/html/canvas-api-image-data.html` | Canvas ImageData API |
| `fixtures/html/css-overflow-comprehensive.html` | Comprehensive CSS overflow test file (#290) |
| `fixtures/html/css-overflow.html` | CSS overflow handling |
| `fixtures/html/webgl-conformance/validateprogram-test.html` | WebGL validateProgram method (#253) |
| `fixtures/html/universal-selector-test.html` | Universal selector (*) in CSSOM parser and matching (#261) |
| `fixtures/html/background-size-position-test.html` | Background-position, background-size and background-repeat (#274) |
| `fixtures/html/background-position-extended-syntax-test.html` | Extended background-position syntax |
| `fixtures/html/background-origin-repeat-test.html` | Background-repeat drawing from clipped area (#280) |
| `fixtures/html/layout-flexbox-alignment.html` | Correct align-items implementation and flexbox layout (#282) |
| `fixtures/html/layout-grid-alignment.html` | CSS Grid layout testing (#282) |
| `fixtures/html/position-absolute-test.html` | Absolute positioning to nearest positioned ancestor (#276) |
| `fixtures/html/document.html` | Document API testing |
| `fixtures/html/elements.html` | HTML element functionality |
| `fixtures/html/dom/document_fragment_test.html` | DocumentFragment API |
| `fixtures/html/dom/document_fragment_clone_test.html` | DocumentFragment cloning |
| `fixtures/html/dom/document_fragment_web_api_test.html` | DocumentFragment Web API compliance |
| `fixtures/html/dom/nodes.html` | DOM node operations |
| `fixtures/html/events.html` | Event handling |
| `fixtures/html/mutation-observers.html` | MutationObserver API |
| `fixtures/html/layout-flexbox-column.html` | Flexbox column layouts |
| `fixtures/html/layout-flexbox-example.html` | Flexbox examples |
| `fixtures/html/layout-flexbox-nested.html` | Nested flexbox layouts |
| `fixtures/html/layout-flexbox-wrap.html` | Flexbox wrapping |
| `fixtures/html/layout-grid-areas.html` | CSS Grid areas |
| `fixtures/html/layout-grid-auto.html` | CSS Grid auto placement |
| `fixtures/html/layout-grid-example.html` | CSS Grid examples |
| `fixtures/html/layout-grid-nested.html` | Nested CSS Grid |
| `fixtures/html/text.html` | Basic text rendering |
| `fixtures/html/text-antialiasing.html` | Text antialiasing |
| `fixtures/html/text-in-flexbox.html` | Text within flexbox layouts |
| `fixtures/html/wide-text.html` | Wide text handling |
| `fixtures/html/css-gradients.html` | CSS gradient support |
| `fixtures/html/style-properties.html` | CSS property testing |
| `fixtures/html/load-stylesheet.html` | Stylesheet loading |
| `fixtures/html/canvas.html` | Basic Canvas API |
| `fixtures/html/webgl.html` | WebGL API testing |
| `fixtures/html/webgl-conformance/bufferdata-size-test.html` | WebGL buffer operations |
| `fixtures/html/webgl-conformance/clearbufferfv-test.html` | WebGL buffer clearing |
| `fixtures/html/three.html` | Three.js integration |
| `fixtures/html/three-example.html` | Three.js examples |
| `fixtures/html/three-loader-gltf.html` | GLTF model loading |
| `fixtures/html/three-loader-draco.html` | Draco compression support |
| `fixtures/html/three-loader-fbx.html` | FBX model loading |
| `fixtures/html/three-loader-usdz.html` | USDZ model loading |
| `fixtures/html/babylon-loader-gltf.html` | Babylon.js GLTF support |
| `fixtures/html/babylon-sample-particles.html` | Particle systems |

## v0.10.0

### New Features & Improvements

#### DOM & Web APIs

- **Custom Fonts Support**: Added support for custom fonts directory via `JSAR_SYSTEM_FONTS_DIR` environment variable (#255)
- **DocumentFragment API**: Complete implementation with ParentNode interface support including `cloneNode()` and proper insert logic (#245, #249, #247)
- **Document API Extensions**: Added support for `createDocumentFragment()` and `createComment()` methods (#235, #233)
- **Script Execution**: Implemented proper blocking script execution order for HTMLScriptElement (#251)
- **Viewport Meta**: Added viewport meta parsing and configuring window width & height (#237)

#### WebGL Enhancements

- **Buffer Operations**: Implemented `bufferData(target, size, usage)` support in WebGL bindings (#248)
- **Vertex Operations**: Added WebGL `vertexAttrib` and `clearBuffer` methods (#205)

#### CSS & Styling

- **CSS Selectors**: Exposed selectors text at style rule level (#239)
- **CSS Filters**: Implemented parsing for CSS `filter` and `backdrop-filter` properties with parameter support (#238)
- **Overflow Handling**: Added support for nested CSS overflow scroll behavior (#229)

#### 3D Graphics & Rendering

- **3D Gaussian Splatting**: Implemented PLY loader for 3D Gaussian Splatting models in `<model>` tag (#223)
- **Layered Rendering**: Added overflow-based and layer-based rendering for web content instances (#225, #220)
- **SDF Text Rendering**: Implemented Signed Distance Field text rendering for improved text anti-aliasing (#198)

#### Developer Tools & Debugging

- **Inspector Integration**: Added content CDP (Chrome DevTools Protocol) proxy and implemented Log domain (#214)
- **Network Monitoring**: Added network service to listen for network state changes (#164)

### Bug Fixes & Performance

#### Graphics & Rendering

- **Shader Performance**: Cached shader's `COMPILE_STATUS` to avoid blocking calls (#254)
- **Buffer Management**: Fixed `dataSize` handling in `BufferDataCommandBufferRequest`
- **Framebuffer Status**: Added proper response handling for `CheckFramebufferStatus()`
- **Texture Bindings**: Fixed multiple texture bindings per target for rendering units (#215)
- **3D Gaussian Splatting**: Optimized GPU performance and compression format to use 1 texel (#212)

#### Build System & Tooling

- **Build Configuration**: Fixed target directory configuration when directory doesn't exist
- **Package Optimization**: Removed deprecated XSML support to reduce package size (#218)

#### Documentation & Development

- **Test Coverage**: Added GLSL patcher test case
- **Documentation**: Updated Rust version notes and navigator.gl type checking guide
- **Development Workflow**: Updated copilot instructions for npm ci usage

## v0.9.0

### New Features & Improvements

#### CSS & Styling Enhancements

- **CSS Transform Parser**: Replaced Rust CSS transform parser with C/C++ implementation for better performance (#176)
- **Background Properties**: Added support for `background-clip`, `background-origin`, and `background-repeat` properties (#135, #125)
- **Background Blend Mode**: Added support for `background-blend-mode` CSS property (#119)
- **Background Images**: Support for `background-image` with `url()` and gradient rendering (#117, #123)
- **CSS Parsing**: Enhanced CSS parsing with specified classes for background-image

#### HTML & DOM Features

- **HTMLInputElement**: Added comprehensive HTMLInputElement support (#148)
- **HTMLIFrameElement**: Added basic HTMLIFrameElement and JSAR.UniversalRenderingServer for unified rendering debugging (#127)
- **Document API**: Added `document.write` and `*.writeln` methods
- **HTML Parsing**: Enhanced HTML parsing with support for unquoted attribute values (#103)
- **Attribute Parsing**: Support for attributes without values and improved PCDATA tag parsing
- **HTML Volume**: Set HTML volume mask depth to 0.05 and added offset to remove gaps between background and borders

#### Rendering & Graphics

- **Layered Rendering**: Support for rendering web content in layers (#172)
- **SVG Support**: Added SVG image rendering capabilities (#169)
- **DPR Support**: Implemented device pixel ratio support in web content rendering
- **Renderer Refactoring**: Complete refactor of the builtin scene renderer
- **Render Pipeline**: Refined render pipeline and improved offscreen pass with computation graph
- **Environment Mapping**: Added environment mapping test files and examples
- **Layer Calculation**: Calculate layer field for each LayoutObject (#174)

#### Inspector & Debugging

- **Chrome DevTools Protocol**: Added basic CDP support with Runtime and Example domains (#121)
- **WebSocket Support**: Added WebSocket support to prepare for Chrome DevTools Protocol integration (#106)
- **MDN Compatibility**: Implemented MDN-compatible browser compatibility data structure (#108)
- **Inspector Logs**: Added `/:id/logs` endpoint for better debugging
- **Enhanced Debugging**: Support for unified rendering backend debugging

#### Examples & Testing

- **Transmute Browser**: Renamed simulator to transmute_browser (#178)
- **Rendering Modes**: Updated args parsing with support for mono/stereo rendering modes (#159)
- **LeaferJS Support**: Added support for LeaferJS base usage in fixtures

#### Build & CI Improvements

- **Deployment Permissions**: Added write permissions for deploy-website workflow
- **Memory Optimization**: Specified Node options to increase V8 memory limits
- **Lint Fixes**: Fixed various lint errors throughout the codebase

## v0.8.2

### Fixes

- Fix the commandbuffer responses dispatching in client-side graphics

## v0.8.1

### Improvements

- Optimize rendering pipeline to support offscreen rendering correctly.
- Speed up the graphics initialization of each content process.

### Fixes

- Correct the FCP timing for actual writing to the color buffer in the shared render target.

## v0.8.0

### New Features & Improvements

#### Rendering Enhancements

- Renderer Pipeline Overhaul
  - Replaced single `onFrame` tick with discrete render passes for finer-grained control over each rendering stage
  - Introduced `UnityRenderPassEvent` and dispatches per-pass events in `OnUnityRenderEvent`
  - Added new lifecycle methods (`onBeforeRendering`, `onOpaquesRenderPass`, `onTransparentsRenderPass`, `onAfterRendering`) in `TrEmbedder` and `TrConstellation`
  - Refactored renderer interfaces and updated OpenGL example to utilize new pass methods
- Context Management
  - Introduced new `ContextGLHost`/`ContextGLApp` classes to replace legacy context storage
  - Added `iterateContentRenderers` API for enhanced renderer iteration
  - Implemented framebuffer attachment helpers with improved debug output including stencil state

#### Inspector & Debugging

- Monitoring Capabilities:
  - Extended inspector with new `/json/statistics` endpoint for better runtime monitoring
  - Enhanced debug output with stencil buffer state information

#### DOM Updates

- Content Manipulation
  - Added support for setting `textContent` property
  - Enhanced DOM element manipulation capabilities

#### XML Processing

- Parser Improvements
  - Added support for parsing script/style tags as plain character data.
  - Improved XML parsing reliability for web content

#### Developer Experience

- Tracing & Diagnostics:
  - Added trace support for generate-document capability in threepio
  - Enhanced debugging and development workflow

#### HTML & CSS Features

- HTML Element Enhancements:
  - Added support for rendering `<canvas>` elements via Web Canvas API
- CSS Features:
  - Added support for `absolute` and `fixed` positioning.
  - Added support for unit in `transform` property.

### Fixes & Optimizations

#### Bug Fixes

- **Connection Management**: Fixed invalid connection handling from forked child processes in hived
- **Build System**: Resolved lint errors in the build process
- **Stencil Buffer**: Addressed stencil buffer issues through renderer refactoring (#92)

#### Maintenance

- **Dependencies**: Upgraded jsar-dom version for improved compatibility
- **Animations**: Updated animation system components

## v0.7.2

### New Features & Improvements

#### Rendering Enhancements

- WebGL Improvements:
  - Optimized WebGL texture handling with anisotropic filtering.
  - Improved support for the `EXT_texture_filter_anisotropic` extension.
  - Enhanced WebGL context attributes for better XR compatibility.

#### DOM Updates

- Event Handling:
  - Added new event constructor types including `MouseEvent`, `PointerEvent`, and `XRSessionEvent`.
  - Enhanced `EventTarget` to provide detailed event type mappings and dispatching.
- Document Features:
  - Introduced `Document.timeline` for animation timelines.
  - Enhanced `Document.appendStyleSheet()` to manage styles dynamically.

#### HTML Enhancements

- Image Element:
  - Improved `<img>` element to handle `onload` and `onerror` events asynchronously.
  - Optimized image decoding with fallback mechanisms for error handling.
- Web Content Updates:
  - Added support for logical dimensions (`logicalWidth` and `logicalHeight`) for web content rendering.
  - Improved device pixel ratio handling in `WebContent`.

### Fixes & Optimizations

#### Performance Optimizations

- Rendering Pipeline:
  - Optimized text and element rendering by marking dirty nodes and reducing unnecessary updates.
  - Improved layout computation efficiency for dynamic content.
- Event Dispatch:
  - Fixed hit testing for mouse events to handle `mouseleave` and `mouseout` scenarios properly.

#### Bug Fixes

- Fixed issues with `CSSStyleDeclaration` caching to ensure seamless style updates.
- Resolved memory leaks in image decoding processes.
- Addressed rendering bugs for logical dimensions in `WebContent`.
- Fixed service memory leaks in destroying content processes.
- Added maximum size for the image and texture at client-side and server-side to prevent out of memory issues.
- Fixed the resource caching issues, now the caching follows the standard HTTP caching rules.

## v0.7.1

### ​​New Features & Improvements​​

#### ​​Layout System Overhaul​​

- ​**​Refactored layout system**​​ to align with the standard **​​CSS Box Model​​**.
  - Added ​**​scrolling and overflow** support​.
  - Introduced ​​**CSS Grid Layout**​​ support.

#### New DOM APIs

- **Node Manipulation**
  - `Node.previousSibling`
  - `Node.replaceChild()`
  - `Node.removeChild()`
  - `Node.cloneNode()`
  - `Node.nextSibling`
  - `Node.insertBefore()`
- ​**​Element Properties & Methods**
  - `Element.innerHTML` (getter & setter)
  - `Element.before()` & `Element.after()`
- **Element Events**
  - click – Fired when the primary button is clicked.
  - mousedown – Fired when the primary button is pressed.
  - mouseup – Fired when the primary button is released.
  - mousemove – Fired when the mouse moves over the element.
  - mouseover – Fired when the mouse enters the element.
  - mouseout – Fired when the mouse leaves the element.
- **​​Document APIs**
  - `Document.querySelector()` & `Document.querySelectorAll()`.
  - `Document.createTextNode()`.
  - Added `DocumentFragment` support.

#### ​​HTML & CSS Enhancements​​

- ​**​Dataset Support​**​ – Added `data-*` attribute handling for HTML elements.
- **​​MutationObserver**​​ – Now supports observing DOM tree changes.
- ​**​New HTML Elements**
  - `<link>` (`HTMLLinkElement`) – Supports loading external CSS via href.
  - `<template>` (`HTMLTemplateElement`)
  - ​​**Semantic Elements**:​​ `<header>`, `<footer>`, `<main>`, `<section>`.
- **CSS Properties**
  - `text-transform` – Supports uppercase, lowercase, and capitalize.

#### ​​Performance Optimizations​​

- **​​ECS (Entity-Component-System):​​**
  - Optimized queryEntities() with a ​​new caching system​​.
- **Rendering**
  - Improved efficiency by **​​skipping rendering of overflow images​​**.

## v0.7.0

- Supports the rendering for HTML elements with high performance, both elements rendering in the two draw calls.
- Supports update the elements in 3D via the CSS transform functions such as `translate`, `rotate`, and `scale`.
- Open as HTML document by default.
- Add inspector support to debug the rendering backend issues.

## v0.6.0

- HTML is introduced as the main entry point to start an application.
  - Include the following elements (element rendering is not working yet):
    - `<audio>`
    - `<body>`
    - `<canvas>`
    - `<div>`
    - `<head>`
    - `<html>`
    - `<img>`
    - `<meta>`
    - `<p>`
    - `<script>`
    - `<span>`
    - `<video>`
  - Add scripting and module system support:
    - Scripting: JavaScript & TypeScript
    - ECMAScript Modules
  - Add Web Canvas API: `HTMLCanvasElement` and `OffscreenCanvas`.
  - Add `location` object to support parsing the request URL.
  - Add `window.open(url)` to open a new window (this depends on the embedder).
  - Add `window.close()` to close the current application.
  - Add `document.head` and `document.body` to access the head and body elements.
  - Add `getElementById(id)` to get an element by its ID.
  - Add `getElementsByTagName(tagName)` and `getElementsByName(name)` to get elements by tag name and name.
- WebGL:
  - Fix the extension `OVR_multiview2` constant values.
  - Implement the WebGL method: `gl.hint()`.
  - Implement the WebGL method: `gl.lineWidth()`.
  - Implement the WebGL method: `gl.texParameterf()`.
- Support running three.js projects in HTML.
- Support running Babylon.js projects in HTML.

## v0.5.0

- Added ability to support Unity Universal RP.
- Added ability to support more stereo rendering modes: `SinglePass`, `SinglePassInstanced`, and `SinglePassMultiview`.
- Fixed the GLES state managment for stencil buffer and color mask.

## v0.4.0

- Added the WebGL 1.0 APIs.
- Added the WebGL 2.0 APIs partially.
- Added the WebXR Device APIs:
  - Rendering: `XRFrame`, `XRView`, `XRViewport`, `XRWebGLLayer`
  - Input Sources: `XRInputSource`, `XRInputSourceArray`, `XRInputSourceEvent`
  - Spaces: `XRSpace`, `XRReferenceSpace`, `XRViewSpace`, `XRLocalSpace`, `XRWorldSpace`
- Added the basic input sources: gaze and hand.
- Added the `audio` implementation with the audio spatialization support.
