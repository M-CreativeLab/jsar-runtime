# JSAR Alpha Changelog

The alpha version of JSAR is the first version of the project.

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
