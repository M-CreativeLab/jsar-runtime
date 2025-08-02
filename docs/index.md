---
layout: home

hero:
  name: "JSAR"
  text: "Spatial Web Browser Engine"
  tagline: Create immersive Web experiences with HTML5, CSS3, and TypeScript.
  image:
    src: /logo-playwebxr.jpg
    alt: JSAR Logo
  actions:
    - theme: brand
      text: Get Started
      link: /manual/quick-start
    - theme: alt
      text: Learn More
      link: /manual/introduction
    - theme: alt
      text: View on GitHub
      link: https://github.com/M-CreativeLab/jsar-runtime

features:
  - icon: 🎯
    title: 3D-First Design
    details: Mix-rendering multiple Web 2D and 3D contents in a single scene with high performance GPU-driven renderer.
  
  - icon: 🌐
    title: Web Standards Compliant
    details: Supports WebXR, WebGL, DOM, CSSOM, and other modern Web standards for seamless compatibility.
  
  - icon: ⚡
    title: High Performance
    details: Built-in ECS architecture achieves GPU-driven rendering for elements with TypeScript-native scripting.

  - icon: 📱
    title: Cross-Platform
    details: Runs on desktop, mobile, and XR devices with consistent performance and functionality.

  - icon: 🛠️
    title: Developer Friendly
    details: Rich debugging tools, hot reload, and comprehensive documentation for smooth development experience.

  - icon: 🔧
    title: Extensible
    details: Plugin system and modular architecture allow for easy customization and extension.
---

## Technology Stack

JSAR provides comprehensive support for modern web technologies and 3D graphics APIs.

### Scripting & Modules
- ✅ **JavaScript** - Full ES2023 support
- ✅ **TypeScript** - Native TypeScript compilation
- ✅ **ECMAScript Module** - Modern module system
- ✅ **WebAssembly** - High-performance WASM support
- ✅ **Web Worker** - Multi-threaded processing

### 3D Graphics
- ✅ **WebGL 1.0** - Fully supported
- ✅ **WebGL 2.0** - Complete implementation
- 🔄 **WebGPU** - In development

### WebXR Device APIs
- ✅ **Spaces & Reference Spaces** - Complete implementation
- ✅ **Stereo Rendering** - Optimized dual-eye rendering
- ✅ **Input Sources** - Controllers and hand tracking
- ✅ **Hand Tracking** - Precise gesture recognition

### Classic Rendering
- 🔄 **DOM API** - Partial implementation
- 🔄 **HTML5** - Core features supported
- 🔄 **CSS3** - Growing feature set
- ✅ **Canvas 2D** - Full 2D rendering support

## Latest Updates

::: info Latest Release
**v0.8.2** - December 2024

- CSS gradient support
- Performance optimizations
- Enhanced developer tools
- Improved documentation

[View Full Changelog](/changelogs/alpha.md)
:::

## Getting Started

Ready to build your first spatial web application? Check out our comprehensive guides:

<div class="custom-block tip">
  <p class="custom-block-title">Quick Start</p>
  <p>Learn the basics and create your first JSAR application in minutes.</p>
  <a href="/manual/quick-start">Start Learning →</a>
</div>

<div class="custom-block tip">
  <p class="custom-block-title">Examples</p>
  <p>Explore practical examples and tutorials to deepen your understanding.</p>
  <a href="/manual/tutorials">View Tutorials →</a>
</div>

## Community

Join our growing community of developers building the future of spatial web:

- 💬 [GitHub Discussions](https://github.com/M-CreativeLab/jsar-runtime/discussions)
- 🐛 [Report Issues](https://github.com/M-CreativeLab/jsar-runtime/issues)
- 📖 [Contributing Guide](/contributing/coding_style_guides)
- 📰 [Blog](/blog) - Technical insights and updates

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