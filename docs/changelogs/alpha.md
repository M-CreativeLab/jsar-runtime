# JSAR Alpha Changelog

The alpha version of JSAR is the first version of the project.

## v0.9.0

### New Features & Improvements

#### Website Enhancements

- Added search bar functionality to improve navigation and content discovery

### Maintenance

- Updated project version to 0.9.0 for official release

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
