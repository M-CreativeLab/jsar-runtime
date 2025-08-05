# Introduction

**JSAR** ([/dʒ:-sar/](https://ipa-reader.xyz/?text=d%CA%92:-sar)) is a Web browser engine designed for the spatial Web. It enables you to create and present immersive Web contents using the modern Web technologies ([HTML5][], [CSS3][], [TypeScript][]) and seamlessly integrate them into mainstream 3D engines like [Unity][], [UnrealEngine][], and other 3D engines.

[HTML5]: https://html.spec.whatwg.org/multipage/
[CSS3]: https://www.w3.org/TR/css-2023/
[TypeScript]: https://www.typescriptlang.org/
[Unity]: https://unity.com/
[UnrealEngine]: https://www.unrealengine.com/

## Spatial Web

The Spatial Web represents the next evolution of the Web, where every HTML element exists in three-dimensional space with real 3D coordinates, rather than being confined to flat 2D screens. This spatial transformation enables developers to freely leverage Web technologies to create truly immersive experiences, seamlessly blending virtual reality (VR), augmented reality (AR), and mixed reality (MR) into both our physical world and any digital virtual environments.

Key characteristics of the Spatial Web include:

- **3D-First Design**: Content and interfaces are designed to exist in 3D space
- **Natural Interaction**: Users can interact with digital content using natural gestures and movements
- **Contextual Awareness**: Digital experiences that understand and respond to their physical surroundings
- **Persistent**: Digital content can remain anchored in physical space across sessions

### Spatial Web on Desktop

The Spatial Web is not limited to XR devices - it's also transforming how we interact with content on traditional desktop computers. On desktop platforms, Spatial Web browsing creates a bridge between 2D and 3D experiences by:

- **Depth-Based Navigation**: Instead of traditional tab-based browsing, content can be organized in 3D space with depth, allowing users to navigate "forward" and "backward" through information layers
- **Spatial Organization**: Web content can be arranged in a three-dimensional workspace, enabling more intuitive organization of multiple pages, documents, and applications
- **Enhanced Visualization**: Data and content can be presented with depth and dimension, making complex information more comprehensible through spatial relationships
- **Natural Interactions**: Even with a mouse and keyboard, users can interact with content in more natural ways by moving through 3D space rather than just clicking through flat pages

This desktop implementation of Spatial Web maintains the core principles of spatial computing while adapting them to traditional input methods, creating a more immersive and intuitive browsing experience without requiring specialized hardware.

## Project Objectives

- Native support for rendering Spatial Web content
- Designed for Web developers, no need to learn new languages or frameworks.
- Ability to render existing Web applications spatially without modifications.

## Key Features

- **Designed for the spatial Web** - Every HTML element (including 2D & 3D) is spatially rendered in the browser.
- **Web Standards Compliant** - Supports modern Web standards, including HTML5, CSS3, DOM, [WebGL][], [WebXR][] and others.
- **High Performance** - Dynamically batches all spatialized HTML elements into ≤10 GPU draw calls per frame.
- **Cross-Platform** - Delivers flawless performance across every platform (desktop, mobile, and XR) with pixel-perfect consistency.
- **Web Developer Friendly** - Deep [Chrome DevTools][] integration for seamless debugging and profiling.

[WebGL]: https://www.khronos.org/webgl/
[WebXR]: https://www.w3.org/TR/webxr/
[Chrome DevTools]: https://developer.chrome.com/docs/devtools/

## Use Cases

Let's explore how JSAR can benefit different types of developers:

### For XR System Developers

XR system developers are those who build and maintain XR operating systems and platforms, such as [Meta][]'s [Quest OS][], [Pico][]'s operating system, or other mixed reality platforms. If you're developing these types of XR systems or platforms, JSAR offers:

- True spatial rendering of HTML elements in 3D space, not just flat browser windows
- High-performance HTML rendering optimized for spatial computing with full depth perception

For example, if you're working on [Quest][] or [Pico][]'s system-level features, you can use JSAR to:

- Build system UI and menus using familiar Web technologies (HTML/CSS/JavaScript)
- Create immersive web-based system applications
- Build spatial computing experiences that leverage web technologies
- Integrate web content browsers into your XR platform

[Meta]: https://about.meta.com/
[Quest OS]: https://www.meta.com/quest/
[Quest]: https://www.meta.com/quest/
[Pico]: https://www.picoxr.com/

### For Web Developers (No 3D/XR Experience Required)

If you're a web developer with traditional HTML/CSS/JavaScript experience:

- Run your existing 2D web applications in spatial environments without modification
- Leverage your current web development skills to create spatial experiences
- Progressive transition to 3D/XR development by only adding CSS properties like `transform: translate3d(0, 1, 0)`

### For WebXR Developers

If you're already working with [WebXR][]:

- Create rich spatial UI components and overlays using standard HTML and CSS
- Seamlessly mix 2D web content with 3D/XR experiences
- Develop cross-platform XR applications with consistent rendering

## Documentation Usage Guide

This documentation is organized to help you quickly find the information you need. Here's how to effectively use it:

### Structure Overview

| Section | Description |
|---------|-------------|
| **Getting Started** | Introduction to JSAR, quick start guide, and basic concepts |
| **Features** | Comprehensive coverage of modules, TypeScript support, graphics libraries ([Babylon.js][], [Three.js][], [VanillaJS][]), media, and advanced features like [WebAssembly][] and [Web Workers][] |
| **Integration** | Guides for integrating JSAR with platforms like Unity and other 3D engines via C/C++ embedder APIs |
| **Developer Tools** | Resources for debugging, preview functionality, and development tools including GUI features and packaging |
| **Tutorials** | Hands-on examples covering everything from Hello World applications to creating materials, meshes, and handling network requests |
| **References** | Detailed documentation on APIs and specifications |

[Babylon.js]: https://www.babylonjs.com/
[Three.js]: https://threejs.org/
[VanillaJS]: http://vanilla-js.com/
[WebAssembly]: https://webassembly.org/
[Web Workers]: https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API

### Writing Style

Our documentation follows these principles:

- **Self-contained sections**: Each section is written to be understood independently
- **Progressive disclosure**: Concepts build upon each other naturally
- **Practical examples**: Real-world code samples accompany explanations
- **Cross-references**: Links to related topics for deeper understanding

We aim to balance comprehensiveness with clarity, ensuring you can quickly find answers while having access to detailed information when needed.
