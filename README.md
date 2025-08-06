<div align="center">
  <img alt="JSAR Poster" src="./JSAR_POSTER.png" height="300">
  <p align="center">
    JSAR: a <b>Browser Engine Library</b> designed for The Spatial Web.
    <br>
    <br>
    <a href="https://m-creativelab.github.io/jsar-runtime/manual/introduction.html">User Manual</a>
    ·
    <a href="https://deepwiki.com/m-creativelab/jsar-runtime">DeepWiki</a>
  </p>

  [![GitHub Release](https://img.shields.io/github/v/release/m-creativelab/jsar-runtime)](https://github.com/m-creativelab/jsar-runtime/releases)
  [![GitHub commit activity](https://img.shields.io/github/commit-activity/m/m-creativelab/jsar-runtime)](https://github.com/M-CreativeLab/jsar-runtime/commits/main/)
  [![GitHub License](https://img.shields.io/github/license/m-creativelab/jsar-runtime)](https://github.com/m-creativelab/jsar-runtime/blob/main/LICENSE)
</div>

# Table of Contents

1. [Introduction](#introduction)
     - [Spatial Web](#spatial-web)
     - [Key Features](#key-features)
2. [Web Compatibility Overview](#web-compatibility-overview)
     - [Request Formats](#supported-url-formats-to-request)
     - [Scripting & Modules](#scripting--modules)
     - [3D Graphics](#3d-graphics)
     - [Rendering Backends](#rendering-backends)
     - [WebXR Device APIs](#webxr-device-apis)
     - [Multimedia APIs](#multimedia-apis)
     - [Canvas](#canvas)
     - [Classic Rendering](#classic-rendering)
3. [Contributing](#contributing)
     - [Prerequisites](#prerequisites)
     - [Build from source](#build-from-source)
4. [Loaders](#loaders)

## Introduction

**JSAR** ([/dʒ:-sar/](https://ipa-reader.xyz/?text=d%CA%92:-sar)) is a **Web browser engine** designed for the spatial Web. It enables you to create and present immersive Web contents using the modern Web technologies ([HTML5][], [CSS3][], [TypeScript][]) and seamlessly integrate them into mainstream 3D engines like [Unity][], [UnrealEngine][], and other 3D engines.

### Spatial Web

The Spatial Web represents the next evolution of the Web, where every HTML element exists in three-dimensional space with real 3D coordinates, rather than being confined to flat 2D screens. This spatial transformation enables developers to freely leverage Web technologies to create truly immersive experiences, seamlessly blending virtual reality (VR), augmented reality (AR), and mixed reality (MR) into both our physical world and any digital virtual environments.

Key characteristics of the Spatial Web include:

- **3D-First Design**: Content and interfaces are designed to exist in 3D space
- **Natural Interaction**: Users can interact with digital content using natural gestures and movements
- **Contextual Awareness**: Digital experiences that understand and respond to their physical surroundings
- **Persistent**: Digital content can remain anchored in physical space across sessions

### Key Features

- **Designed for the spatial Web** - Every HTML element (including 2D & 3D) is spatially rendered in the browser.
- **Web Standards Compliant** - Supports modern Web standards, including HTML5, CSS3, DOM, [WebGL][], [WebXR][] and others.
- **High Performance** - Dynamically batches all spatialized HTML elements into ≤10 GPU draw calls per frame.
- **Cross-Platform** - Delivers flawless performance across every platform (desktop, mobile, and XR) with pixel-perfect consistency.
- **Web Developer Friendly** - Deep [Chrome DevTools][] integration for seamless debugging and profiling.

[WebGL]: https://www.khronos.org/webgl/
[WebXR]: https://www.w3.org/TR/webxr/
[Chrome DevTools]: https://developer.chrome.com/docs/devtools/

## Web Compatibility Overview

### Supported URL Formats to Request

Like the classic Web browser, it supports not only the HTML document, but also lots of formats, such as SVG, 3D models, etc. The following formats is expected to support:

| Format   | Status      | Recommended Use Case       |
| -------- | ----------- | -------------------------- |
| HTML     | Partially   | Browsing classic Web pages |
| GLTF     | OK          | Displaying 3D models       |
| JPEG/PNG | OK          | Displaying images          |
| SVG      | OK          | Displaying SVG images      |

### Scripting & Modules

| API                   | Status | Test Suite |
| --------------------- | ------ | ---------- |
| [JavaScript][]        | Ok     | -          |
| [TypeScript][]        | Ok     | -          |
| [ECMAScript Module][] | Ok     | -          |
| [WebAssembly][]       | Ok     | -          |
| [Web Worker][]        | Ok     | -          |

[JavaScript]: https://developer.mozilla.org/en-US/docs/Web/JavaScript
[TypeScript]: https://www.typescriptlang.org/
[ECMAScript Module]: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Modules
[WebAssembly]: https://webassembly.org/
[Web Worker]: https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API

### 3D Graphics

| API        | Status      | Test Suite                                                             |
| ---------- | ----------- | ---------------------------------------------------------------------- |
| [WebGL1][] | Ok          | https://github.com/KhronosGroup/WebGL/tree/main/sdk/tests/conformance  |
| [WebGL2][] | Ok          | https://github.com/KhronosGroup/WebGL/tree/main/sdk/tests/conformance2 |
| [WebGPU][] | Not started | https://github.com/gpuweb/cts                                          |

[WebGL1]: https://registry.khronos.org/webgl/specs/latest/1.0/
[WebGL2]: https://registry.khronos.org/webgl/specs/latest/2.0/
[WebGPU]: https://www.w3.org/TR/webgpu/

### Rendering Backends

The following are supported renderer backends:

| Backend       | OS      | Status      | Test Suite |
| ------------- | ------- | ----------- | ---------- |
| [OpenGLES3][] | Android | Ok          | -          |
| [OpenGL][]    | macOS   | Partially   | -          |
| [Metal][]     | macOS   | Not started | -          |
| [D3D11][]     | Windows | Not started | -          |

[OpenGL]: https://www.opengl.org/
[OpenGLES3]: https://www.khronos.org/opengles/
[Metal]: https://developer.apple.com/metal/
[D3D11]: https://learn.microsoft.com/en-us/windows/win32/direct3d11/atoc-dx-graphics-direct3d-11

### WebXR Device APIs

| API                                                                                                               | Status | Test Suite |
| ----------------------------------------------------------------------------------------------------------------- | ------ | ---------- |
| [Spaces and Reference Spaces](https://developer.mozilla.org/en-US/docs/Web/API/WebXR_Device_API/Spatial_tracking) | Ok     | -          |
| [Stereo Rendering](https://developer.mozilla.org/en-US/docs/Web/API/WebXR_Device_API/Rendering)                   | Ok     | -          |
| [Inputs and Input Sources](https://developer.mozilla.org/en-US/docs/Web/API/WebXR_Device_API/Inputs)              | Ok     | -          |

#### Supported Input Sources

| Input Source    | Status          | Target Ray Mode        | Gamepad | Profiles |
| --------------- | --------------- | ---------------------- | ------- | -------- |
| gaze            | Ok              | gaze                   | No      | -        |
| hand(2)         | Ok              | tracked-pointer        | No      | -        |
| main controller | Ok              | tracked-pointer/screen | Yes     | -        |
| transient       | Not implemented | transient-pointer      | No      | -        |

> Note: Gamepad is not supported in the current version.

#### Features

| Feature            | Status          | Test Suite |
| ------------------ | --------------- | ---------- |
| Hit Test           | Not implemented | -          |
| Anchors            | Not implemented | -          |
| Hand Tracking      | Ok              | -          |
| Eye Tracking       | Not implemented | -          |
| Depth Sensing      | Not implemented | -          |
| Face Tracking      | Not implemented | -          |
| Body Tracking      | Not implemented | -          |
| Environment Probes | Not implemented | -          |
| Light Estimation   | Not implemented | -          |

### Multimedia APIs

In Web multimedia, there are the following APIs that JSAR would support:

- Playback: play audio and video, and use Web Audio API to create audio effects.
- Capture: capture audio and video from the device.

#### Playback

| API                    | Status      | Test Suite |
| ---------------------- | ----------- | ---------- |
| [`HTMLAudioElement`][] | Ok          | -          |
| [`HTMLVideoElement`][] | Not started | -          |
| [Web Audio API][]      | Not started | -          |

[`HTMLAudioElement`]: https://developer.mozilla.org/en-US/docs/Web/API/HTMLAudioElement
[`HTMLVideoElement`]: https://developer.mozilla.org/en-US/docs/Web/API/HTMLVideoElement
[Web Audio API]: https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API

#### Capture

| API                 | Status      | Test Suite |
| ------------------- | ----------- | ---------- |
| [`MediaDevices`][]  | Not started | -          |
| [`MediaRecorder`][] | Not started | -          |

[`MediaDevices`]: https://developer.mozilla.org/en-US/docs/Web/API/MediaDevices
[`MediaRecorder`]: https://developer.mozilla.org/en-US/docs/Web/API/MediaRecorder

### Canvas

| API                      | Status      | Test Suite |
| ------------------------ | ----------- | ---------- |
| [`RenderingContext2D`][] | Ok          | -          |
| `PDFRenderingContext`    | Not started | -          |
| `HTMLRenderingContext`   | Not started | -          |

[`RenderingContext2D`]: https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D

> Note: `PDFRenderingContext` and `HTMLRenderingContext` are introduced by JSAR, which is used to customize the PDF and HTML rendering.

### Classic Rendering

| API               | Status      | Test Suite |
| ----------------- | ----------- | ---------- |
| [DOM API][]       | Partially   | -          |
| [HTML5][]         | Partially   | -          |
| [CSS3][]          | Partially   | -          |
| [Web Component][] | Not started | -          |

[DOM API]: https://developer.mozilla.org/en-US/docs/Web/API/Document_Object_Model
[HTML5]: https://developer.mozilla.org/en-US/docs/Web/Guide/HTML/HTML5
[CSS3]: https://developer.mozilla.org/en-US/docs/Web/CSS/CSS3
[Web Component]: https://developer.mozilla.org/en-US/docs/Web/Web_Components

## Contributing

To contribute to the project, you need to follow the steps below to build this project from source.

### Prerequisites

- [Node.js](https://nodejs.org/en/) v18.16.1
- [Rust](https://www.rust-lang.org/) v1.70.0 or later

And you need to add the following targets to your Rust toolchain by your need:

```sh
$ rustup target add aarch64-linux-android
$ rustup target add aarch64-apple-darwin
$ rustup target add x86_64-apple-darwin
$ rustup target add x86_64-pc-windows-msvc
```

### Build from source

Firstly, you need to install the dependencies and build the JSAR JavaScript bundle which is used in the runtime.

```sh
$ npm install && make jsbundle
```

Then you can build the runtime library:

```sh
$ make darwin    # for macOS universal library
$ make android   # for Android-aarch64 library
```

If you want to do a clean build, you can run:

```sh
$ make darwin CLEAN=yes
```

Supported parameters:

- `CLEAN`: Clean the build directory before building.
- `RELEASE`: Build the release version of the library.
- `INSPECTOR`: Enable the inspector.

## Loaders

The loaders are more convenient way to integrate JSAR into your applications, the following loaders are planned to be developed:

| Loader                | Status      | Description                                     |
| --------------------- | ----------- | ----------------------------------------------- |
| [jsar-loader-unity][] | Ok          | Integrates JSAR into Unity applications         |
| jsar-loader-unreal    | Not started | Integrates JSAR into Unreal Engine applications |

[jsar-loader-unity]: https://github.com/M-CreativeLab/jsar-loader-unity

> To develop a new loader, you could see the [Embedder API Documentation](docs/api/embedder.md) for more details.

[WebXR]: https://immersiveweb.dev/
[WebGL]: https://get.webgl.org/
[Unity]: https://unity.com/
[UnrealEngine]: https://www.unrealengine.com/
[HTML5]: https://www.w3.org/TR/html5/
[CSS3]: https://www.w3.org/Style/CSS/Overview.en.html
[TypeScript]: https://www.typescriptlang.org/
