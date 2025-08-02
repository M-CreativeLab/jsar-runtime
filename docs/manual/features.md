# Features

In this chapter, we will introduce the development features, this helps the application developers to learn what features are available in JSAR and how to use them.

## Scripting & Modules

JSAR has implemented the scripting engine (based on [v8 engine][v8]) and module system, which allows developers to write scripts in JavaScript for application development. Based on the above works, JSAR also supported [TypeScript][], the developers can use TypeScript without any configuration and compilation at runtime.

### Key Features
- Native TypeScript support without transpilation
- ES6+ module system
- WebAssembly integration
- Web Workers for parallel processing

[v8]: https://v8.dev/
[TypeScript]: https://www.typescriptlang.org/

## Space(3D) Development

JSAR works with the 3d libraries like [Three.js][] and [Babylon.js][], which allows developers to create 3D applications with ease. And also provides an article to support [Vanilla JS][vanillajs] developers to support their own 3D libraries in JSAR.

### Supported Frameworks
- Three.js integration for WebGL-based 3D graphics
- Babylon.js support for advanced 3D rendering
- Vanilla JavaScript for custom implementations

[Three.js]: https://threejs.org/
[Babylon.js]: https://www.babylonjs.com/
[vanillajs]: https://vanilla-js.com/

## Multimedia

Multimedia is an essential part of the application experience, especially in XR.

JSAR implemented the multimedia-related APIs to help developers to use images, audio and video in your applications. And also JSAR provides advanced features to enhance the multimedia experience in space, such as a simplified way to create 3D audio.

### Media Support
- Advanced image processing and display
- Spatial audio system with 3D positioning
- Video playback with spatial integration

## Application Debugging

Every application is a standalone process in JSAR, and this process will start a debugging server to provide the debugging interface for the developers. The debugging server is based on the [Chrome DevTools Protocol][cdp], which allows developers to debug the application in the browser.

### Debugging Features
- Chrome DevTools integration
- Real-time debugging in spatial environments
- Performance profiling for XR applications

[cdp]: https://chromedevtools.github.io/devtools-protocol/