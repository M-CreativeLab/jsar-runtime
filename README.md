# JSAR Runtime

The JSAR is a library for embedding [WebXR][], [WebGL][] and classic Web applications into [Unity][], [UnrealEngine][], RealityKit and standalone.

> Standalone means you could also use JSAR to pack your WebXR application as a standalone application.

## Overview

JSAR is a 3d-first Web rendering engine, that is easily to *embed* and use. Developers are able to use the parts of Web APIs to build the spatial computing applications still.

### Rendering Backends

The followings are supported renderer backend:

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

### Request Formats

Like the classic Web browser, it supports not only the HTML document, but also lots of formats, such as PDF, SVG, etc. The following formats are JSAR is expected to support:

| Format   | Status      | Recommended Use Case         |
| -------- | ----------- | ---------------------------- |
| XSML     | Ok          | Using WebXR applications     |
| HTML     | Partially   | Browsering classic Web pages |
| PDF      | Not started | Reading PDF documents        |
| SVG      | Not started | Displaying SVG images        |
| GLTF     | Ok          | Displaying 3D models         |
| JPEG/PNG | Ok          | Viewing images               |

> XSML: namely eXtensible Spatial Markup Language, is a new format that is designed for spatial applications for HTML developers.

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

| API               | Status           | Test Suite |
| ----------------- | ---------------- | ---------- |
| [DOM API][]       | Work in progress | -          |
| [HTML5][]         | Not started      | -          |
| [CSS3][]          | Not started      | -          |
| [Web Component][] | Not started      | -          |

[DOM API]: https://developer.mozilla.org/en-US/docs/Web/API/Document_Object_Model
[HTML5]: https://developer.mozilla.org/en-US/docs/Web/Guide/HTML/HTML5
[CSS3]: https://developer.mozilla.org/en-US/docs/Web/CSS/CSS3
[Web Component]: https://developer.mozilla.org/en-US/docs/Web/Web_Components

## Getting Started

> TODO

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
$ make darwin # for macOS universal library
$ make android # for Android aarch64 library
$ make windows # for Windows x86_64 library
```

If you want to do a clean build, you can run:

```sh
$ make darwin clean=yes
```

## Loaders

The loaders are more convenient way to integrate JSAR into your applications, the following loaders are planned to be developed:

| Loader                | Status      | Description                                     |
| --------------------- | ----------- | ----------------------------------------------- |
| [jsar-loader-unity][] | Ok          | Integrates JSAR into Unity applications         |
| jsar-loader-unreal    | Not started | Integrates JSAR into Unreal Engine applications |
| jsar-loader-visionos  | Not started | Integrates JSAR into visionOS applications      |

[jsar-loader-unity]: https://github.com/M-CreativeLab/jsar-loader-unity

> To develop a new loader, you could see the [Embedder API Documentation](docs/api/embedder.md) for more details.

## License

[Apache License 2.0](LICENSE)

[WebXR]: https://immersiveweb.dev/
[WebGL]: https://get.webgl.org/
[Unity]: https://unity.com/
[UnrealEngine]: https://www.unrealengine.com/
