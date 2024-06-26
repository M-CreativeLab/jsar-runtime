# JSAR Runtime

The JSAR is a runtime SDK for embedding [WebXR][], [WebGL][] and Web 2D applications into [Unity][], [UnrealEngine][], RealityKit and standalone.

> Standalone means you could also use JSAR to pack your WebXR application as a standalone application.

It's a 3d-first web rendering engine, that is easily to embed and use, and it supports the following Web APIs:

- [ ] Graphics
  - [x] WebGL1
  - [x] WebGL2
  - [ ] WebGL Exts
  - [ ] WebGPU
- [ ] WebXR Device API
  - [x] Basic rendering
  - [x] Input sources
  - [ ] Features
- [ ] Web Audio API
- [ ] Canvas
  - [x] RenderingContext2D
  - [ ] PDFRenderingContext
  - [ ] HTMLRenderingContext
- [ ] 2D Application Rendering
  - [ ] HTML5
  - [ ] CSS3
  - [ ] Web Component

And the followings are supported renderer backend:

- [x] OpenGL & OpenGLES
- [ ] Metal
- [ ] DirectX11

## Getting Started

TODO

## Development

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

- [jsar-loader-unity](https://github.com/M-CreativeLab/jsar-loader-unity)
- jsar-loader-unreal
- jsar-loader-ios
- jsar-loader-android

## License

[Apache License 2.0](LICENSE)

[WebXR]: https://immersiveweb.dev/
[WebGL]: https://get.webgl.org/
[Unity]: https://unity.com/
[UnrealEngine]: https://www.unrealengine.com/
