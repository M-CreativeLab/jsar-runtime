---
layout: home

hero:
  name: "JSAR"
  text: "Spatial Web Browser Engine"
  tagline: "Empowering Web developers to craft immersive experiences with the full power of modern web technologies — HTML5, CSS3, and TypeScript."
  actions:
    - theme: brand
      text: Get Started
      link: /manual/quick-start
    - theme: alt
      text: Ask on DeepWiki
      link: https://deepwiki.com/M-CreativeLab/jsar-runtime
    - theme: alt
      text: View on GitHub
      link: https://github.com/M-CreativeLab/jsar-runtime

header: false
feature3DEffect: true

features:
  - icon: 🎯
    title: Design for Spatial Web
    details: Every HTML element is spatially rendered in the browser, powered by JSAR.

  - icon: 🌐
    title: Web Standards Compliant
    details: Supports modern Web standards, including HTML5, CSS3, DOM, WebGL, and WebXR.

  - icon: ⚡
    title: High Performance
    details: Dynamically batches all spatialized HTML elements into ≤10 GPU draw calls per frame.

  - icon: 📱
    title: Cross-Platform
    details: Delivers flawless performance across every platform (desktop, mobile, and XR) with pixel-perfect consistency.

  - icon: 🛠️
    title: Web Developer Friendly
    details: Deep Chrome DevTools integration for seamless debugging and profiling.

  - icon: 🔧
    title: Extensible
    details: An extensible plugin system and modular architecture unlock limitless customization.
---

## Web Compatibility Overview

JSAR provides comprehensive support for modern Web standards. For a complete browser compatibility report, please visit our [Browser Compatibility Data](/api/browser-compat-data.html).

### Scripting & Modules

- [Completed] **JavaScript** - Full ES2023 support
- [Completed] **TypeScript** - Native TypeScript compilation
- [Completed] **ECMAScript Module** - Modern module system
- [Completed] **WebAssembly** - High-performance WASM support
- [Completed] **Web Worker** - Multi-threaded processing

### 3D Graphics

- [Completed] **WebGL 1.0** - Fully supported
- [Completed] **WebGL 2.0** - Complete implementation
- [In Progress] **WebGPU** - In development

### WebXR Device APIs

- [Completed] **Spaces & Reference Spaces** - Complete implementation
- [Completed] **Stereo Rendering** - Optimized dual-eye rendering
- [Completed] **Input Sources** - Controllers and hand tracking
- [Completed] **Hand Tracking** - Precise gesture recognition

### Classic Rendering

- [In Progress] **DOM API** - Partial implementation
- [In Progress] **HTML5** - Core features supported
- [In Progress] **CSS3** - Growing feature set
- [Completed] **Canvas 2D** - Full 2D rendering support

## Latest Updates

<LatestRelease />

## Getting Started

Ready to build your first spatial web application? Check out our comprehensive guides:

<div class="custom-block tip">
  <p class="custom-block-title">Quick Start</p>
  <p>Learn the basics and create your first JSAR application in minutes.</p>
  <a href="manual/quick-start">Start Learning →</a>
</div>

<div class="custom-block tip">
  <p class="custom-block-title">Examples</p>
  <p>Explore practical examples and tutorials to deepen your understanding.</p>
  <a href="manual/tutorials/helloworld-babylonjs">View Tutorials →</a>
</div>

## Community

Join our growing community of developers building the future of spatial web:

- 💬 [GitHub Discussions](https://github.com/M-CreativeLab/jsar-runtime/discussions)
- 🐛 [Report Issues](https://github.com/M-CreativeLab/jsar-runtime/issues)
- 📖 [Contributing Guide](contributing/coding_style_guides)
- 📰 [Blog](blog) - Technical insights and updates

<style>
.VPHero .VPImage {
  max-width: 280px;
  border-radius: 16px;
}

.custom-block.tip {
  border-color: var(--vp-c-brand-1);
}

.custom-block.tip .custom-block-title {
  color: var(--vp-c-brand-1);
}
</style>